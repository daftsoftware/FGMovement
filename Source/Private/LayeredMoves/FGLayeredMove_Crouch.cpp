// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "LayeredMoves/FGLayeredMove_Crouch.h"
#include "FGMovementCVars.h"
#include "MoverComponent.h"
#include "MoverDataModelTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGLayeredMove_Crouch)

FFGLayeredMove_Crouch::FFGLayeredMove_Crouch()
{
	DurationMs = 0.f;
	MixMode = EMoveMixMode::OverrideVelocity;
}

bool FFGLayeredMove_Crouch::GenerateMove(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, const UMoverComponent* MoverComp, UMoverBlackboard* SimBlackboard, FProposedMove& OutProposedMove)
{
	const FMoverDefaultSyncState* SyncState = StartState.SyncState.SyncStateCollection.FindDataByType<FMoverDefaultSyncState>();
	check(SyncState);

	checkf(MixMode == EMoveMixMode::OverrideVelocity, TEXT("Only OverrideVelocity is supported for crouch move."));
	const FVector PriorVelocityWS = SyncState->GetVelocity_WorldSpace();

	OutProposedMove.LinearVelocity = PriorVelocityWS * FG::CVars::CrouchSpeedMult; // Rescale velocity by crouch speed.

	return true;
}

FLayeredMoveBase* FFGLayeredMove_Crouch::Clone() const
{
	FFGLayeredMove_Crouch* CopyPtr = new FFGLayeredMove_Crouch(*this);
	return CopyPtr;
}

void FFGLayeredMove_Crouch::NetSerialize(FArchive& Ar)
{
	FLayeredMoveBase::NetSerialize(Ar);
}

UScriptStruct* FFGLayeredMove_Crouch::GetScriptStruct() const
{
	return FFGLayeredMove_Crouch::StaticStruct();
}

FString FFGLayeredMove_Crouch::ToSimpleString() const
{
	return FString::Printf(TEXT("Crouch"));
}

void FFGLayeredMove_Crouch::AddReferencedObjects(FReferenceCollector& Collector)
{
	FLayeredMoveBase::AddReferencedObjects(Collector);
}
