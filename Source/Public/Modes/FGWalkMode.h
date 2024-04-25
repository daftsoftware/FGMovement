// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "MovementMode.h"
#include "FGWalkMode.generated.h"

struct FFGMoverInputCmd;

UCLASS()
class FGMOVEMENT_API UFGWalkMode final : public UBaseMovementMode
{
public:
	GENERATED_BODY()

	UFGWalkMode();

	//~ Begin UBaseMovementMode
	void OnGenerateMove(const FMoverTickStartData& StartState, const FMoverTimeStep& TimeStep, FProposedMove& OutProposedMove) const override;
	void OnSimulationTick(const FSimulationTickParams& Params, FMoverTickEndData& OutputState) override;
	//~ End UBaseMovementMode

	bool TryJump(const FFGMoverInputCmd* InputCmd, FMoverTickEndData& OutputState);
};
