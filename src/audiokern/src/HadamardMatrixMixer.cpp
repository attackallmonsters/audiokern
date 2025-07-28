#include "HadamardMatrixMixer.h"
#include <cmath>
#include <cstring>

HadamardMatrixMixer::HadamardMatrixMixer(int max)
{
    maxBuffers = max;

    buffersL.resize(maxBuffers);
    buffersR.resize(maxBuffers);
    tempBuffersL.resize(maxBuffers);
    tempBuffersR.resize(maxBuffers);

    std::random_device rd;
    rng.seed(rd());
}

void HadamardMatrixMixer::initializeObject(size_t size)
{
    bufferCount = clamp(static_cast<int>(size), static_cast<int>(2), maxBuffers);

    matrix.assign(bufferCount, std::vector<float>(bufferCount, 0.0f));

    DSP::log("%i", bufferCount);

    for (int i = 0; i < bufferCount; ++i)
    {
        tempBuffersL[i].initialize("HadamardMatrix_tmpBuffer_L_" + std::to_string(i) + getName(), DSP::blockSize);
        tempBuffersR[i].initialize("HadamardMatrix_tmpBuffer_R_" + std::to_string(i) + getName(), DSP::blockSize);
    }

    setMode(Mode::Linear);
}

void HadamardMatrixMixer::setMode(Mode mode)
{
    currentMode = mode;

    switch (currentMode)
    {
    case Mode::Random:
        generateRandom();
        break;
    case Mode::Linear:
        generateLinear();
        break;
    case Mode::MirrorPairs:
        generateMirrorPairs();
        break;
    }
}

void HadamardMatrixMixer::setInputBuffer(int n, DSPSampleBuffer &bufL, DSPSampleBuffer &bufR)
{
    int target = clamp(n, 0, bufferCount - 1);

    buffersL[target] = bufL;
    buffersR[target] = bufR;
}

void HadamardMatrixMixer::generateRandom()
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (int i = 0; i < bufferCount; ++i)
        for (int j = 0; j < bufferCount; ++j)
            matrix[i][j] = (i == j) ? 0.0f : dist(rng);
}

void HadamardMatrixMixer::generateLinear()
{
    float scale = 1.0f / std::sqrt(static_cast<float>(bufferCount));
    for (int i = 0; i < bufferCount; ++i)
        for (int j = 0; j < bufferCount; ++j)
            matrix[i][j] = (i == j) ? 0.0f : ((i + j) % 2 == 0 ? scale : -scale);
}

void HadamardMatrixMixer::generateMirrorPairs()
{
    for (int i = 0; i < bufferCount; ++i)
    {
        for (int j = 0; j < bufferCount; ++j)
        {
            if (i == j)
            {
                matrix[i][j] = 0.0f;
            }
            else if (j == bufferCount - i - 1)
            {
                matrix[i][j] = 1.0f;
            }
            else
            {
                matrix[i][j] = 0.0f;
            }
        }

        // Optional: for center element in odd-sized matrix, average left/right
        if (bufferCount % 2 == 1 && i == bufferCount / 2)
        {
            int left = i - 1;
            int right = i + 1;
            if (left >= 0)
                matrix[i][left] = 0.5f;
            if (right < bufferCount)
                matrix[i][right] = 0.5f;
        }
    }
}

void HadamardMatrixMixer::processBlock()
{
    // Step 1: Copy current buffer contents into temp
    for (int i = 0; i < bufferCount; ++i)
    {
        tempBuffersL[i].copy(buffersL[i]);
        tempBuffersR[i].copy(buffersR[i]);
    }

    // Step 2: Apply matrix mixing to each output buffer
    for (int i = 0; i < bufferCount; ++i)
    {
        float* outL = buffersL[i].data();
        float* outR = buffersR[i].data();

        for (size_t s = 0; s < DSP::blockSize; ++s)
        {
            float accL = 0.0f;
            float accR = 0.0f;

            for (int j = 0; j < bufferCount; ++j)
            {
                float gain = matrix[i][j];
                accL += gain * tempBuffersL[j][s];
                accR += gain * tempBuffersR[j][s];
            }

            outL[s] = accL;
            outR[s] = accR;
        }
    }
}

void HadamardMatrixMixer::processBlock(DSPObject *dsp)
{
    HadamardMatrixMixer *self = static_cast<HadamardMatrixMixer *>(dsp);
    self->processBlock();
}
