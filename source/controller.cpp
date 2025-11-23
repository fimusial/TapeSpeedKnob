#include "vstgui/plugin-bindings/vst3editor.h"
#include "base/source/fstreamer.h"
#include "vstgui/uidescription/uicontentprovider.h"

#include "cids.h"
#include "controller.h"

using namespace Steinberg;
using namespace Vst;

namespace TSK
{
    tresult PLUGIN_API TapeSpeedKnobController::initialize(FUnknown* context)
    {
        tresult result = EditControllerEx1::initialize(context);
        if (result != kResultOk)
        {
            return result;
        }

        parameters.addParameter(
            STR16("Bypass"), nullptr, 1, 0, ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass, Bypass);

        parameters.addParameter(
            STR16("Speed"), nullptr, 0, 1, ParameterInfo::kCanAutomate, Speed);

        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobController::terminate()
    {
        return EditControllerEx1::terminate();
    }

    tresult PLUGIN_API TapeSpeedKnobController::setComponentState(IBStream* state)
    {
        if (!state)
        {
            return kInvalidArgument;
        }

        IBStreamer streamer(state, kLittleEndian);

        bool bypass;
        if (!streamer.readBool(bypass))
        {
            bypass = false;
        }
        if (setParamNormalized(Bypass, bypass) == kResultFalse)
        {
            return kResultFalse;
        }

        double speed;
        if (!streamer.readDouble(speed))
        {
            speed = 1.0;
        }
        if (setParamNormalized(Speed, speed) == kResultFalse)
        {
            return kResultFalse;
        }

        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobController::setState(IBStream* state)
    {
        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobController::getState(IBStream* state)
    {
        return kResultOk;
    }

    IPlugView* PLUGIN_API TapeSpeedKnobController::createView(FIDString name)
    {
        if (FIDStringsEqual(name, ViewType::kEditor))
        {
            return (IPlugView*)new VSTGUI::VST3Editor(
                new VSTGUI::UIDescription(
                    (VSTGUI::IContentProvider*)new VSTGUI::MemoryContentProvider(uiDescription, 4459),
                    nullptr),
                this,
                "view");
        }

        return nullptr;
    }

    const char* TapeSpeedKnobController::uiDescription =
R"(
{
    "vstgui-ui-description": {
        "version": "1",
        "bitmaps": {},
        "fonts": {},
        "colors": {},
        "gradients": {},
        "control-tags": {
            "Bypass": "256",
            "Speed": "257"
        },
        "custom": {},
        "templates": {
            "view": {
                "attributes": {
                    "background-color": "~ BlackCColor",
                    "background-color-draw-style": "filled",
                    "class": "CViewContainer",
                    "mouse-enabled": "true",
                    "opacity": "1",
                    "origin": "0, 0",
                    "size": "128, 128",
                    "transparent": "false",
                    "wants-focus": "false"
                },
                "children": {
                    "CKnob": {
                        "attributes": {
                            "angle-range": "270",
                            "angle-start": "135",
                            "circle-drawing": "false",
                            "class": "CKnob",
                            "control-tag": "Speed",
                            "corona-color": "~ TransparentCColor",
                            "corona-dash-dot": "false",
                            "corona-dash-dot-lengths": "1.26,0.1",
                            "corona-drawing": "false",
                            "corona-from-center": "false",
                            "corona-inset": "8",
                            "corona-inverted": "false",
                            "corona-line-cap-butt": "true",
                            "corona-outline": "true",
                            "corona-outline-width-add": "2",
                            "handle-color": "~ WhiteCColor",
                            "handle-line-width": "3",
                            "handle-shadow-color": "~ WhiteCColor",
                            "knob-range": "200",
                            "opacity": "1",
                            "origin": "10, 10",
                            "size": "110, 110",
                            "skip-handle-drawing": "false",
                            "transparent": "false",
                            "value-inset": "8.5",
                            "wants-focus": "false",
                            "wheel-inc-value": "0.05",
                            "zoom-factor": "1.5"
                        }
                    },
                    "CTextEdit": {
                        "attributes": {
                            "back-color": "~ WhiteCColor",
                            "background-offset": "0, 0",
                            "class": "CTextEdit",
                            "control-tag": "Speed",
                            "font": "~ NormalFont",
                            "font-antialias": "false",
                            "font-color": "~ BlackCColor",
                            "frame-color": "~ GreyCColor",
                            "frame-width": "2",
                            "immediate-text-change": "false",
                            "opacity": "1",
                            "origin": "40, 100",
                            "placeholder-title": "1.0000",
                            "round-rect-radius": "12",
                            "secure-style": "false",
                            "shadow-color": "~ GreyCColor",
                            "size": "50, 20",
                            "style-3D-in": "false",
                            "style-3D-out": "false",
                            "style-doubleclick": "false",
                            "style-no-draw": "false",
                            "style-no-frame": "false",
                            "style-no-text": "false",
                            "style-round-rect": "false",
                            "style-shadow-text": "false",
                            "text-alignment": "center",
                            "text-inset": "0, 0",
                            "text-rotation": "0",
                            "text-shadow-offset": "1, 1",
                            "transparent": "false",
                            "truncate-mode": "tail",
                            "value-precision": "4",
                            "wants-focus": "false",
                            "wheel-inc-value": "0.05"
                        }
                    }
                }
            }
        }
    }
}
)";
}
