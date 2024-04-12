// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "Core/FGPawn.h"
#include "FGExamplePawn.generated.h"

class UFGExampleAssetData;

/**
 * Example pawn implementation that inherits AFGPawn.
 * This class is not intended to be inherited from, it's just
 * an example of using the FG pawn class, to use it yourself
 * you should be inheriting AFGPawn yourself.
 */
UCLASS()
class AFGExamplePawn final : public AFGPawn
{
	GENERATED_BODY()
public:

	//~ Begin FGPawn
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void PawnClientRestart() override;
	//~ End FGPawn
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UFGExampleAssetData> DefaultAssetData;
};
