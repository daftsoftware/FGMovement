// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "InputActionValue.h"
#include "MoverSimulationTypes.h"
#include "GameFramework/Pawn.h"
#include "FGPawn.generated.h"

struct FFGMoverInputCmd;
class UFGMoverComponent;
class UCapsuleComponent;
class UCameraComponent;

/**
 * Custom implementation of a pawn that uses the FG movement component
 * without any additional logic.
 */
UCLASS()
class FGMOVEMENT_API AFGPawn : public APawn, public IMoverInputProducerInterface
{
	GENERATED_BODY()
public:

	AFGPawn();

	//~ Begin APawn
	virtual void Tick(float DeltaSeconds) override;
	//~ End APawn
	
	virtual void Move(const FInputActionValue& Value);
	virtual void MoveCompleted(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void Jump();
	virtual void JumpCompleted();
	virtual void Crouch();
	virtual void CrouchCompleted();

	//~ Begin IMoverInputProducerInterface
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd) override;
	//~ End IMoverInputProducerInterface

	UFGMoverComponent*	GetMoverComponent() const { return MoverComponent; }
	UCapsuleComponent*	GetCapsuleComponent() const { return CapsuleComponent; }
	UCameraComponent*	GetCameraComponent() const { return CameraComponent; }

private:

	UPROPERTY(Category = Movement, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFGMoverComponent> MoverComponent;
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	// @TODO: This seems redundant, why aren't we just caching an entire input cmd?
	FVector3d	LastAffirmativeMoveInput	= FVector3d::ZeroVector;	// Movement input (intent or velocity) the last time we had one that wasn't zero
	FVector3d	CachedMoveInputIntent		= FVector3d::ZeroVector;
	FVector3d	CachedMoveInputVelocity		= FVector3d::ZeroVector;
	FRotator3d	CachedTurnInput				= FRotator3d::ZeroRotator;
	FRotator3d	CachedLookInput				= FRotator3d::ZeroRotator;
	bool		JumpButtonDown				= false;
	bool		CrouchButtonDown				= false;
};