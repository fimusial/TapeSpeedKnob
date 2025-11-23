#pragma once

#include "vector"

using namespace std;

namespace TSK
{
    class TapeBuffers
    {
    public:
        TapeBuffers(int channelCount);
        ~TapeBuffers();

        void push(int channelIndex, size_t sampleCount, float* samples32, double* samples64);
        void clear();

        double getCurrentSample(int channelIndex);
        void movePlayhead(double speed);

    private:
        struct Chunk
        {
            size_t size = 0;
            double* samples = nullptr;
        };

        int channelCount = 0;
        vector<vector<Chunk>> channels;
        size_t playheadChunk = 0;
        double playhead = 0.0;

        static constexpr size_t InitChannelCapacity = 256;
        static constexpr size_t MaxChunkSize = 65536;
    };
}