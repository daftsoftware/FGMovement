﻿// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/FGMovementUtils.h"
#include "FGMovementCVars.h"
#include "FGMovementDefines.h"
#include "MoveLibrary/MovementUtilsTypes.h"
#include "MoverComponent.h"
#include "Core/FGMoverComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Logging/StructuredLog.h"
#include "MoveLibrary/MovementUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGMovementUtils)

void UFGMovementUtils::ApplyDamping(UMoverComponent* MoverComponent, FProposedMove& Move, float DeltaTime)
{
	double Speed = Move.LinearVelocity.Size();
	double DragFactor = UKismetMathLibrary::NormalizeToRange(FMath::Max(FG::CVars::SlipFactor, Speed), 0.0, FG::CVars::GroundSpeed);
	double Drag = FG::CVars::GroundDamping * DragFactor;
	
	Move.LinearVelocity += -Move.LinearVelocity * Drag * DeltaTime;
}

void UFGMovementUtils::ApplyAcceleration(UMoverComponent* MoverComponent, FProposedMove& Move, float DeltaTime, FVector DirectionIntent, float DesiredSpeed)
{
	const double Acceleration = DesiredSpeed * FG::CVars::AirAcceleration * DeltaTime;
	const FVector DesiredVelocity = DirectionIntent * DesiredSpeed;

	const double ProjectedCurrentVelocity = Move.LinearVelocity | DirectionIntent;
	const double MissingSpeed = FMath::Max(DesiredSpeed - ProjectedCurrentVelocity, 0.0);
	const double ScaledAcceleration = Acceleration * (MissingSpeed / DesiredSpeed);
	
	Move.LinearVelocity += DirectionIntent * ScaledAcceleration;

	if(FG::CVars::DrawMovementDebug)
	{
		auto* MoverComp = Cast<UFGMoverComponent>(MoverComponent);
		
		// Draw desired velocity.
		DrawDebugLine(
			MoverComp->GetWorld(),
			MoverComp->GetFeetLocation(),
			MoverComp->GetFeetLocation() + DesiredVelocity,
			FColor::Green,
			false,
			-1,
			0,
			1.0f);
		
		// Draw final velocity.
		DrawDebugLine(
			MoverComp->GetWorld(),
			MoverComp->GetFeetLocation(),
			MoverComp->GetFeetLocation() + Move.LinearVelocity,
			FColor::Emerald,
			false,
			-1,
			0,
			1.0f);
	}
}
