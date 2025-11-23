#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

using namespace Steinberg;
using namespace Vst;

namespace TSK
{
    class TapeSpeedKnobController : public EditControllerEx1
    {
    public:
        TapeSpeedKnobController() = default;
        ~TapeSpeedKnobController() SMTG_OVERRIDE = default;

        static FUnknown* createInstance(void* context)
        {
            return (IEditController*)new TapeSpeedKnobController();
        }

        tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
        tresult PLUGIN_API terminate() SMTG_OVERRIDE;
        tresult PLUGIN_API setComponentState(IBStream* state) SMTG_OVERRIDE;
        IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;
        tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
        tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;

        static const char* uiDescription;

        DEFINE_INTERFACES
        END_DEFINE_INTERFACES (EditController)
        DELEGATE_REFCOUNT (EditController)
    };
}
