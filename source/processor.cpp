#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "memory"

#include "cids.h"
#include "processor.h"

using namespace Steinberg;
using namespace Vst;

namespace TSK
{
    TapeSpeedKnobProcessor::TapeSpeedKnobProcessor()
    {
        setControllerClass(kTapeSpeedKnobControllerUID);
    }

    TapeSpeedKnobProcessor::~TapeSpeedKnobProcessor()
    {
        if (tapeBuffers)
        {
            delete tapeBuffers;
        }
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::initialize(FUnknown* context)
    {
        processContextRequirements = IProcessContextRequirements::kNeedTransportState;

        tresult result = AudioEffect::initialize(context);
        if (result != kResultOk)
        {
            return result;
        }

        addAudioInput(STR16("Stereo In"), SpeakerArr::kStereo);
        addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::terminate()
    {
        realTimeTransfer.clear_ui();
        return AudioEffect::terminate();
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::setBusArrangements(
        SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
    {
        if (numIns != 1 || numOuts != 1 || inputs[0] != outputs[0])
        {
            return kResultFalse;
        }

        tresult result = AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
        if (result != kResultOk)
        {
            return result;
        }

        int newChannelCount = SpeakerArr::getChannelCount(outputs[0]);
        if (channelCount > 0 && newChannelCount > channelCount)
        {
            return kResultFalse;
        }

        channelCount = newChannelCount;
        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::canProcessSampleSize(int32 symbolicSampleSize)
    {
        return symbolicSampleSize == kSample32 || symbolicSampleSize == kSample64 ? kResultTrue : kResultFalse;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::setupProcessing(ProcessSetup& newSetup)
    {
        return AudioEffect::setupProcessing(newSetup);
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::setActive(TBool state)
    {
        if (state && !tapeBuffers)
        {
            tapeBuffers = new TapeBuffers(channelCount);
        }
        else if (!state && tapeBuffers)
        {
            delete tapeBuffers;
            tapeBuffers = nullptr;
        }

        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::setState(IBStream* state)
    {
        if (!state)
        {
            return kInvalidArgument;
        }

        IBStreamer streamer(state, kLittleEndian);

        if (!streamer.readBool(bypass))
        {
            bypass = false;
        }

        unique_ptr<double> speed = make_unique<double>();
        if (!streamer.readDouble(*speed))
        {
            *speed = 1.0;
        }

        realTimeTransfer.transferObject_ui(std::move(speed));
        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::getState(IBStream* state)
    {
        if (!state)
        {
            return kInvalidArgument;
        }

        IBStreamer streamer(state, kLittleEndian);

        if (!streamer.writeBool(bypass))
        {
            return kResultFalse;
        }

        if (!streamer.writeDouble(speedParameter.getValue()))
        {
            return kResultFalse;
        }

        return kResultOk;
    }

    tresult PLUGIN_API TapeSpeedKnobProcessor::process(ProcessData& data)
    {
        if (!tapeBuffers)
        {
            return kResultFalse;
        }

        beginParameterChanges(data.inputParameterChanges);
        processSamples(data);
        endParameterChanges();
        return kResultOk;
    }

    void TapeSpeedKnobProcessor::beginParameterChanges(IParameterChanges* changes)
    {
        realTimeTransfer.accessTransferObject_rt(
            [this](const double& speed)
            {
                speedParameter.setValue(speed);
            });

        if (!changes)
        {
            return;
        }

        int count = changes->getParameterCount();
        for (int i = 0; i < count; i++)
        {
            IParamValueQueue* queue = changes->getParameterData(i);
            if (!queue)
            {
                continue;
            }

            if (queue->getParameterId() == Bypass)
            {
                int sampleOffset;
                ParamValue value;
                queue->getPoint(queue->getPointCount() - 1, sampleOffset, value);
                bypass = value > 0;
            }

            if (queue->getParameterId() == Speed)
            {
                speedParameter.beginChanges(queue);
            }
        }
    }

    void TapeSpeedKnobProcessor::endParameterChanges()
    {
        speedParameter.endChanges();
    }

    void TapeSpeedKnobProcessor::processSamples(ProcessData& data)
    {
        if (data.numInputs == 0 || data.numSamples == 0)
        {
            tapeBuffers->clear();
            return;
        }

        bool playing = !data.processContext || (data.processContext->state & ProcessContext::kPlaying);
        bool is32 = processSetup.symbolicSampleSize == kSample32;
        bool is64 = processSetup.symbolicSampleSize == kSample64;

        if (bypass || !playing)
        {
            for (int channel = 0; channel < channelCount; channel++)
            {
                for (int sample = 0; sample < data.numSamples && is32; sample++)
                {
                    data.outputs[0].channelBuffers32[channel][sample] = data.inputs[0].channelBuffers32[channel][sample];
                }

                for (int sample = 0; sample < data.numSamples && is64; sample++)
                {
                    data.outputs[0].channelBuffers64[channel][sample] = data.inputs[0].channelBuffers64[channel][sample];
                }
            }

            data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
            tapeBuffers->clear();
            return;
        }

        for (int channel = 0; channel < channelCount && is32; channel++)
        {
            tapeBuffers->push(channel, data.numSamples, data.inputs[0].channelBuffers32[channel], nullptr);
        }

        for (int channel = 0; channel < channelCount && is64; channel++)
        {
            tapeBuffers->push(channel, data.numSamples, nullptr, data.inputs[0].channelBuffers64[channel]);
        }

        for (int sample = 0; sample < data.numSamples; sample++)
        {
            for (int channel = 0; channel < channelCount && is32; channel++)
            {
                data.outputs[0].channelBuffers32[channel][sample] = tapeBuffers->getCurrentSample(channel);
            }

            for (int channel = 0; channel < channelCount && is64; channel++)
            {
                data.outputs[0].channelBuffers64[channel][sample] = tapeBuffers->getCurrentSample(channel);
            }

            tapeBuffers->movePlayhead(speedParameter.getValue());
            speedParameter.advance(1);
        }

        return;
    }
}
