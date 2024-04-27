// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "FGExamplePawn.h"
#include "FGExampleAssetData.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGExamplePawn)

void AFGExamplePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	const UFGExampleAssetData* AssetData = DefaultAssetData.LoadSynchronous();
	checkf(AssetData, TEXT("Invalid Input Data!"));
	
	if (auto* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		const UInputAction* MoveAction = AssetData->InputActionMove.LoadSynchronous();
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::MoveCompleted);

		const UInputAction* LookAction = AssetData->InputActionLook.LoadSynchronous();
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		const UInputAction* JumpAction = AssetData->InputActionJump.LoadSynchronous();
		EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::JumpCompleted);

		const UInputAction* DuckAction = AssetData->InputActionCrouch.LoadSynchronous();
		EIC->BindAction(DuckAction, ETriggerEvent::Triggered, this, &ThisClass::Crouch);
		EIC->BindAction(DuckAction, ETriggerEvent::Completed, this, &ThisClass::CrouchCompleted);
	}
}

void AFGExamplePawn::PawnClientRestart()
{
	Super::PawnClientRestart();

	const UFGExampleAssetData* AssetData = DefaultAssetData.LoadSynchronous();
	checkf(AssetData, TEXT("Invalid Input Data!"));

	if (const auto* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings(); // PawnClientRestart can run multiple times, clear out leftover mappings.
			Subsystem->AddMappingContext(AssetData->InputMappingContext.LoadSynchronous(), 0);
		}
	}
}
