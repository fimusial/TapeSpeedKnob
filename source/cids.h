#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

using namespace Steinberg;
using namespace Vst;

namespace TSK
{
    static const FUID kTapeSpeedKnobProcessorUID(0x785563D1, 0xE81F54F6, 0xAD7B3E4B, 0xE86824D5);
    static const FUID kTapeSpeedKnobControllerUID(0xE44D17C4, 0xB36A54E2, 0xABCC0EAE, 0xE61A8BFC);

    #define TapeSpeedKnobVST3Category "Fx"

    enum ParameterIds : ParamID
    {
        Bypass = 256,
        Speed
    };
}
