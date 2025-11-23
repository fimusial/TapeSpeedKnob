#include "tapebuffers.h"

namespace TSK
{
    TapeBuffers::TapeBuffers(int channelCount)
        : channelCount(channelCount), channels(channelCount, vector<Chunk>())
    {
        for (vector<Chunk>& channel : channels)
        {
            channel.reserve(InitChannelCapacity);
        }
    }

    TapeBuffers::~TapeBuffers()
    {
        for (vector<Chunk>& channel : channels)
        {
            for (Chunk& chunk : channel)
            {
                delete[] chunk.samples;
            }
        }
    }

    void TapeBuffers::push(int channelIndex, size_t sampleCount, float* samples32, double* samples64)
    {
        if (channelIndex < 0 || channelIndex >= channelCount || sampleCount > MaxChunkSize)
        {
            return;
        }

        vector<Chunk>& channel = channels[channelIndex];
        if (channel.size() == 0 || channel.back().size + sampleCount > MaxChunkSize)
        {
            channel.emplace_back();
        }

        Chunk& chunk = channel.back();
        if (!chunk.samples)
        {
            chunk.samples = new double[MaxChunkSize];
        }

        for (size_t i = 0; i < sampleCount && samples32; i++)
        {
            chunk.samples[chunk.size++] = samples32[i];
        }

        for (size_t i = 0; i < sampleCount && samples64; i++)
        {
            chunk.samples[chunk.size++] = samples64[i];
        }
    }

    void TapeBuffers::clear()
    {
        playhead = 0.0;
        playheadChunk = 0;

        for (vector<Chunk>& channel : channels)
        {
            for (Chunk& chunk : channel)
            {
                delete[] chunk.samples;
            }

            channel.clear();
        }
    }

    double TapeBuffers::getCurrentSample(int channelIndex)
    {
        if (channelIndex < 0 || channelIndex >= channelCount)
        {
            return 0.0;
        }

        vector<Chunk>& channel = channels[channelIndex];
        size_t chunkCount = channel.size();
        if (playheadChunk >= chunkCount)
        {
            return 0.0;
        }

        Chunk& chunk = channel[playheadChunk];
        size_t playheadIndex = size_t(playhead);
        if (playheadIndex >= chunk.size)
        {
            return 0.0;
        }

        double sampleA = chunk.samples[playheadIndex];
        bool lastSample = playheadIndex == chunk.size - 1;
        bool lastChunk = playheadChunk == chunkCount - 1;
        if (lastSample && lastChunk)
        {
            return sampleA;
        }

        double sampleB = lastSample
            ? channel[playheadChunk + 1].samples[0]
            : chunk.samples[playheadIndex + 1];

        return (sampleB - sampleA) * (playhead - playheadIndex) + sampleA;
    }

    void TapeBuffers::movePlayhead(double speed)
    {
        if (speed < 0.0 || 1.0 < speed)
        {
            return;
        }

        double nextPlayhead = playhead + speed;
        if (nextPlayhead >= MaxChunkSize)
        {
            nextPlayhead -= MaxChunkSize;
            playheadChunk++;
        }

        playhead = nextPlayhead;
    }
}