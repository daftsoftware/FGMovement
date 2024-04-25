// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/FGMoverComponent.h"
#include "Modes/FGWalkMode.h"
#include "Modes/FGAirMode.h"
#include "FGMovementDefines.h"
#include "Components/CapsuleComponent.h"
#include "Logging/StructuredLog.h"
#include "MoveLibrary/FloorQueryUtils.h"
#include "DrawDebugHelpers.h"
#include "FGMovementCVars.h" 

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGMoverComponent)

UFGMoverComponent::UFGMoverComponent()
{
	// Clear stock movement modes.
	MovementModes.Reset();
	MovementModes.Add(FG::Modes::Walk, CreateDefaultSubobject<UFGWalkMode>(TEXT("FGWalkMode")));
	MovementModes.Add(FG::Modes::Air, CreateDefaultSubobject<UFGAirMode>(TEXT("FGAirMode")));
	StartingMovementMode = FG::Modes::Air;
}

FVector UFGMoverComponent::GetFeetLocation()
{
	if(auto* Capsule = Cast<UCapsuleComponent>(UpdatedComponent))
	{
		return Capsule->GetComponentLocation() + (-FVector::UpVector * Capsule->GetScaledCapsuleHalfHeight());
	}
	return FVector::ZeroVector;
}

void UFGMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if ENABLE_DRAW_DEBUG
	if(FG::CVars::DrawMovementDebug)
	{
		FFloorCheckResult LastFloorResult;
		GetSimBlackboard()->TryGet(CommonBlackboard::LastFloorResult, LastFloorResult);

		bool TouchingCollision = LastFloorResult.bWalkableFloor;

		FQuat PlaneRot = LastFloorResult.bWalkableFloor ?	
			LastFloorResult.HitResult.ImpactNormal.ToOrientationQuat() :
			FVector::UpVector.ToOrientationQuat();

		// Draw floor plane.
		DrawDebugBox(
			GetWorld(),
			GetFeetLocation(),
			FVector(0.0f, 50.0f, 50.0f),
			PlaneRot,
			TouchingCollision ? FColor::Green : FColor::Red,
			false,
			-1,
			0,
			1.0f);

		if(LastFloorResult.bWalkableFloor)
		{
			// Draw floor normal.
			DrawDebugLine(
				GetWorld(),
				GetFeetLocation(),
				GetFeetLocation() + LastFloorResult.HitResult.ImpactNormal * 50.f,
				FColor::Blue,
				false,
				-1,
				0,
				1.0f);
		}

		// Draw per frame velocity.
		DrawDebugLine(
			GetWorld(),
			GetFeetLocation(),
			GetFeetLocation() + (GetVelocity() * DeltaTime),
			FColor::White,
			false,
			-1,
			0,
			1.0f);
	}
#endif

	UE_LOGFMT(LogMover, Display, "Current Movement Mode - {Name}", GetMovementModeName());
	UE_LOGFMT(LogMover, Display, "IsOnGround - {Grounded}", IsOnGround());
}

bool UFGMoverComponent::IsAirborne() const
{
	if (bHasValidCachedState)
	{
		return CachedLastSyncState.MovementMode == FG::Modes::Air;
	}
	return false;
}

bool UFGMoverComponent::IsOnGround() const
{
	if (bHasValidCachedState)
	{
		return CachedLastSyncState.MovementMode == FG::Modes::Walk;
	}

	return false;
}
