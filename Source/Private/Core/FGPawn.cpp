// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/FGPawn.h"
#include "Core/FGMoverComponent.h"
#include "Core/FGDataModel.h"
#include "FGMovementDefines.h"
#include "InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Logging/StructuredLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGPawn)

AFGPawn::AFGPawn()
{
	SetReplicateMovement(false);
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetHiddenInGame(false);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CapsuleComponent);
	CameraComponent->bUsePawnControlRotation = true;
	
	MoverComponent = CreateDefaultSubobject<UFGMoverComponent>(TEXT("MoverComponent"));
}

void AFGPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static constexpr double LookRateYaw = 150.0;	// Degs/Sec
	static constexpr double LookRatePitch = 150.0;	// Degs/Sec
	
	AddControllerYawInput(CachedLookInput.Yaw * LookRateYaw * DeltaSeconds);
	AddControllerPitchInput(CachedLookInput.Pitch * LookRatePitch * DeltaSeconds);

	CachedLookInput = FRotator3d::ZeroRotator;
}

void AFGPawn::Move(const FInputActionValue& Value)
{
	const FVector MoveVector = Value.Get<FVector>();

	// @TODO: Base impl is clamping here, why?
	CachedMoveInputIntent = FVector3d(MoveVector);
}

void AFGPawn::MoveCompleted(const FInputActionValue& Value)
{
	CachedMoveInputIntent = FVector::ZeroVector;
}

void AFGPawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	CachedLookInput.Yaw = CachedTurnInput.Yaw = LookVector.X;
	CachedLookInput.Pitch = CachedTurnInput.Pitch = LookVector.Y;
}

void AFGPawn::Jump()
{
	JumpButtonDown = true;
}

void AFGPawn::JumpCompleted()
{
	JumpButtonDown = false;
}

void AFGPawn::Crouch()
{
	CrouchButtonDown = true;
}

void AFGPawn::CrouchCompleted()
{
	CrouchButtonDown = false;
}

// Produce input is used to build an input cmd for the frame.
void AFGPawn::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd)
{
	FFGMoverInputCmd& CharacterInputs = OutInputCmd.InputCollection.FindOrAddMutableDataByType<FFGMoverInputCmd>();

	if (!GetController())
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
		{
			static const FFGMoverInputCmd DoNothingInput;
			CharacterInputs = DoNothingInput;
		}
		return;
	}

	FRotator IntentRotation = GetControlRotation();
	IntentRotation.Pitch = 0.0f;
	IntentRotation.Roll = 0.0f;

	UE_LOGFMT(LogMover, Display, "Local - JumpButtonDown {Jumping}", JumpButtonDown);

	UE_LOGFMT(LogMover, Display, "Input - {Vector}", CachedMoveInputIntent.ToString());

	CharacterInputs.ControlRotation = GetControlRotation();
	CharacterInputs.bUsingMovementBase = false;
	CharacterInputs.OrientationIntent = IntentRotation.Vector();
	CharacterInputs.bIsJumpPressed = JumpButtonDown;
	CharacterInputs.bIsCrouchPressed = CrouchButtonDown;
	CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, CachedMoveInputIntent);
}
