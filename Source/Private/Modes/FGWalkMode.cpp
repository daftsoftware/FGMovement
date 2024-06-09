// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Modes/FGWalkMode.h"
#include "Core/FGMovementUtils.h"
#include "Core/FGDataModel.h"
#include "LayeredMoves/FGLayeredMove_Crouch.h"
#include "Transitions/FGCrouchCheck.h"
#include "FGMovementCVars.h"
#include "Core/FGMoverComponent.h"
#include "FGMovementDefines.h"

#include "DefaultMovementSet/LayeredMoves/BasicLayeredMoves.h"
#include "MoveLibrary/MovementUtils.h"
#include "MoveLibrary/FloorQueryUtils.h"
#include "MoverComponent.h"
#include "Components/CapsuleComponent.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGWalkMode)

UFGWalkMode::UFGWalkMode()
{
	Transitions.Add(CreateDefaultSubobject<UFGCrouchCheck>(TEXT("DuckCheck")));
}

/**
 * Generate a single substep of movement for the mode - remember this is sub-stepping against
 * network prediction plugins tick and NOT the game thread tick. This function would typically
 * process a single input command, generating a proposed move to be processed in the next tick.
 * Note that a "Move" does not encapsulate movement for a simulation tick, but rather a "sub-tick".
 * 
 * @param StartState - Inputs, sync state, and other data from the start of the tick.
 * @param TimeStep - The time step for this substep.
 * @param OutProposedMove - The proposed move to provide to the next tick that runs.
 */
void UFGWalkMode::OnGenerateMove(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove) const
{
	const FFGMoverInputCmd* CharacterInputs = StartState.InputCmd.InputCollection.FindDataByType<FFGMoverInputCmd>();
	const FMoverDefaultSyncState* StartingSyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(StartingSyncState);

	OutProposedMove.LinearVelocity = StartingSyncState->GetVelocity_WorldSpace();
	const float DeltaTime = TimeStep.StepMs * 0.001f;
	
	UFGMovementUtils::ApplyDamping(CastChecked<UFGMoverComponent>(GetMoverComponent()), OutProposedMove, DeltaTime);

	constexpr float TurningRateLimit = 5000.0f;
	
	// @TODO: This is going to break if the player isn't Z up. Works for now.
    OutProposedMove.AngularVelocity = UMovementUtils::ComputeAngularVelocity(
    	StartingSyncState->GetOrientation_WorldSpace(),
    	CharacterInputs->GetOrientationIntentDir_WorldSpace().ToOrientationRotator(),
    	DeltaTime,
    	TurningRateLimit);
    
    UE_LOGFMT(LogMover, Display, "Angular Velocity: {AngVel}", *OutProposedMove.AngularVelocity.ToString());
    
    OutProposedMove.DirectionIntent = CharacterInputs->GetOrientationIntentDir_WorldSpace();
    	
	FFloorCheckResult FloorResult;
	GetMoverComponent()->GetSimBlackboard()->TryGet(CommonBlackboard::LastFloorResult, FloorResult);

	FVector MoveInputWS = OutProposedMove.DirectionIntent.ToOrientationRotator().RotateVector(CharacterInputs->GetMoveInput());

	FVector ProjectedMove = FVector::VectorPlaneProject(MoveInputWS, FloorResult.HitResult.ImpactNormal);
	ProjectedMove.Normalize();

	UFGMovementUtils::ApplyAcceleration(CastChecked<UFGMoverComponent>(GetMoverComponent()), OutProposedMove, DeltaTime, ProjectedMove, FG::CVars::GroundSpeed);

    UE_LOGFMT(LogMover, Display, "Linear Velocity: {LinVel}", *OutProposedMove.LinearVelocity.ToString());
}

/**
 * Actual network prediction plugin tick - Evolution of the sync state.
 * A DeltaTime is calculated from the last NPP tick, and then we figure out how many "sub-ticks"
 * happened in that time, aggregate them together using the MovementRecord, and then apply the
 * moves to the Pawn. This is where we actually see the moves from OnGenerateMove happen.
 * 
 * @param Params - Input parameters for the movement tick.
 * @param OutputState - The final state of the mover after the tick.
 */
void UFGWalkMode::OnSimulationTick(const FSimulationTickParams& Params, FMoverTickEndData& OutputState)
{
	const FMoverTickStartData& StartState = Params.StartState;
	USceneComponent* UpdatedComponent = Params.UpdatedComponent;
	UPrimitiveComponent* UpdatedPrimitive = Params.UpdatedPrimitive;
	FProposedMove ProposedMove = Params.ProposedMove;

	const FFGMoverInputCmd* CharacterInputs = StartState.InputCmd.InputCollection.FindDataByType<FFGMoverInputCmd>();
	const FMoverDefaultSyncState* StartingSyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(StartingSyncState);

	FMoverDefaultSyncState& OutputSyncState = OutputState.SyncState.SyncStateCollection.FindOrAddMutableDataByType<FMoverDefaultSyncState>();

	const float DeltaSeconds = Params.TimeStep.StepMs * 0.001f;

	// Instantaneous movement changes that are executed and we exit before consuming any time
	if (ProposedMove.bHasTargetLocation && FG::AttemptTeleport(UpdatedComponent, ProposedMove.TargetLocation, UpdatedComponent->GetComponentRotation(), *StartingSyncState, OutputState))
	{
		OutputState.MovementEndState.RemainingMs = Params.TimeStep.StepMs; 	// Give back all the time
		return;
	}

	if(TryJump(CharacterInputs, OutputState))
	{
		OutputState.MovementEndState.NextModeName = FG::Modes::Air;
	}

	FMovementRecord MoveRecord;
	MoveRecord.SetDeltaSeconds(DeltaSeconds);

	UMoverBlackboard* SimBlackboard = GetBlackboard_Mutable();
	SimBlackboard->Invalidate(CommonBlackboard::LastFloorResult); // Flush last floor result.

	constexpr float FloorSweepDist = 1.0f;
	constexpr float MaxWalkSlopeCosine = 0.71f;

	FFloorCheckResult NewFloor;
	
	// If we don't have cached floor information, we need to search for it again
	UFloorQueryUtils::FindFloor(UpdatedComponent, UpdatedPrimitive, FloorSweepDist,
		MaxWalkSlopeCosine, UpdatedPrimitive->GetComponentLocation(), NewFloor);

	SimBlackboard->Set(CommonBlackboard::LastFloorResult, NewFloor);

	OutputSyncState.MoveDirectionIntent = (ProposedMove.bHasDirIntent ? ProposedMove.DirectionIntent : FVector::ZeroVector);

	// Use the orientation intent directly. If no intent is provided, use last frame's orientation. Note that we are assuming rotation changes can't fail. 
	const FRotator StartingOrient = StartingSyncState->GetOrientation_WorldSpace();
	FRotator TargetOrient = StartingOrient;

	bool bIsOrientationChanging = false;

	// Apply orientation changes (if any)
	if (!ProposedMove.AngularVelocity.IsZero())
	{
		TargetOrient += (ProposedMove.AngularVelocity * DeltaSeconds);
		bIsOrientationChanging = (TargetOrient != StartingOrient);
	}
	
	FVector MoveDelta = ProposedMove.LinearVelocity * DeltaSeconds;
	const FQuat OrientQuat = TargetOrient.Quaternion();
	FHitResult Hit(1.f);

	if (!MoveDelta.IsNearlyZero() || bIsOrientationChanging)
	{
		UMovementUtils::TrySafeMoveUpdatedComponent(UpdatedComponent, UpdatedPrimitive, MoveDelta, OrientQuat, true, Hit, ETeleportType::None, MoveRecord);
	}

	if (NewFloor.bWalkableFloor && UpdatedPrimitive)
	{
		// @TODO: Stinky forward port from NPP implementation - Fix me.
		// In the NPP implementation we did a really dirty hack and basically just bodged the
		// slide vector to the expected travel distance. This means when you run into a wall
		// your movement speed isn't clamped depending on the angle of the wall.
		// However we can do it way better now, although it may mean not using this util.
		UMovementUtils::TryMoveToSlideAlongSurface(
			UpdatedComponent,
			UpdatedPrimitive,
			GetMoverComponent(),
			MoveDelta,
			1.f - Hit.Time,
			OrientQuat,
			Hit.Normal,
			Hit,
			false,
			MoveRecord);
	}
	else
	{
		OutputState.MovementEndState.NextModeName = FG::Modes::Air;
	}

	FG::CaptureFinalState(UpdatedComponent, MoveRecord, *StartingSyncState, OutputSyncState, DeltaSeconds);
}

bool UFGWalkMode::TryJump(const FFGMoverInputCmd* InputCmd, FMoverTickEndData& OutputState)
{
	if(!InputCmd->bIsJumpPressed)
	{
		return false;
	}

	TSharedPtr<FLayeredMove_JumpImpulse> JumpMove = MakeShared<FLayeredMove_JumpImpulse>();
	JumpMove->UpwardsSpeed = FG::CVars::JumpForce;
	OutputState.SyncState.LayeredMoves.QueueLayeredMove(JumpMove);
	OutputState.MovementEndState.NextModeName = FG::Modes::Air;

	return true;
}
