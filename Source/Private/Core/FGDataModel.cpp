// Copyright (c) 2024 Daft Software
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Core/FGDataModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FGDataModel)

FFGMoverInputCmd::FFGMoverInputCmd()
	: bIsCrouchPressed(false)
{}

FMoverDataStructBase* FFGMoverInputCmd::Clone() const
{
	FFGMoverInputCmd* CopyPtr = new FFGMoverInputCmd(*this);
	return CopyPtr;
}

bool FFGMoverInputCmd::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar.SerializeBits(&bIsCrouchPressed, 1);
	return FCharacterDefaultInputs::NetSerialize(Ar, Map, bOutSuccess);
}

void FFGMoverInputCmd::ToString(FAnsiStringBuilderBase& Out) const
{
	FCharacterDefaultInputs::ToString(Out);
	Out.Appendf("bIsCrouchPressed: %i\n", bIsCrouchPressed);
}
