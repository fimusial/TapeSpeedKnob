#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "public.sdk/source/vst/utility/sampleaccurate.h"
#include "public.sdk/source/vst/utility/rttransfer.h"

#include "cids.h"
#include "tapebuffers.h"

using namespace Steinberg;
using namespace Vst;

namespace TSK
{
    class TapeSpeedKnobProcessor : public AudioEffect
    {
    public:
        TapeSpeedKnobProcessor();
        ~TapeSpeedKnobProcessor() SMTG_OVERRIDE;

        static FUnknown* createInstance(void* context)
        {
            return (IAudioProcessor*)new TapeSpeedKnobProcessor;
        }

        tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
        tresult PLUGIN_API terminate() SMTG_OVERRIDE;
        tresult PLUGIN_API setBusArrangements(
            SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;
        tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;
        tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) SMTG_OVERRIDE;
        tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
        tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
        tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;
        tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;

    private:
        void beginParameterChanges(IParameterChanges* changes);
        void processSamples(ProcessData& data);
        void endParameterChanges();

        bool bypass = false;
        int channelCount = 0;
        RTTransferT<double> realTimeTransfer;
        SampleAccurate::Parameter speedParameter { ParameterIds::Speed, 1.0 };
        TapeBuffers* tapeBuffers = nullptr;
    };
}
