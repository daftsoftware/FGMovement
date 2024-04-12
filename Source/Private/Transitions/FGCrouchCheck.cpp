// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Transitions/FGCrouchCheck.h"
#include "LayeredMoves/FGLayeredMove_Crouch.h"
#include "Core/FGDataModel.h"
#include "FGMovementDefines.h"

#include "MoverSimulationTypes.h"
#include "MoverComponent.h"

void UFGCrouchCheck::OnTrigger(const FSimulationTickParams& Params)
{
	TSharedPtr<FFGLayeredMove_Crouch> DuckMove = MakeShared<FFGLayeredMove_Crouch>();
	Params.MoverComponent->QueueLayeredMove(DuckMove);
}

FTransitionEvalResult UFGCrouchCheck::OnEvaluate(const FSimulationTickParams& Params) const
{
	FTransitionEvalResult EvalResult = FTransitionEvalResult::NoTransition;

	if (const FFGMoverInputCmd* InputCmd = Params.StartState.InputCmd.InputCollection.FindDataByType<FFGMoverInputCmd>())
	{
		if (InputCmd->bIsCrouchPressed)
		{
			//EvalResult.NextMode = FG::Modes::Walk;
		}
	}

	return EvalResult;
}
