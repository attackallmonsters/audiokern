#pragma once

#include "DSPObject.h"
#include "clamp.h"
#include <vector>
#include <cstddef>
#include <random>

/**
 * @brief Cross-feedback matrix for stereo delay networks in reverb structures.
 *
 * Generates different structured matrices (random, linear, mirror-pair based)
 * and applies them in-place to audio buffers with block-based processing.
 */
class HadamardMatrixMixer : public DSPObject
{
public:
    /**
     * @brief Matrix generation modes.
     */
    enum class Mode
    {
        Random,     ///< Random off-diagonal coefficients, diagonal is zero.
        Linear,     ///< Alternating ±1 pattern, zero diagonal.
        MirrorPairs ///< Delay[i] receives feedback from Delay[N-i-1].
    };

    /**
     * @brief Create an empty matrix.
     */
    HadamardMatrixMixer(int max);

    /**
     * @brief Set matrix generation mode.
     *
     * @param mode Feedback structure to use.
     */
    void setMode(Mode mode);

    void setInputBuffer(int n, DSPSampleBuffer &bufL, DSPSampleBuffer &bufR);

protected:
    /**
     * @brief Initializes the matrix mixer with a given name and size, allocating memory.
     * @param name Name of the mixer.
     * @param size Number of samples.
     */
    void initializeObject(size_t size) override;

private:
    /**
     * @brief Static DSP callback wrapper.
     *
     * Called by the DSP system for block-based processing.
     *
     * @param dsp Pointer to the CombDelay instance (cast from DSPObject*).
     */
    static void processBlock(DSPObject *dsp);

    void processBlock();

    std::vector<std::vector<host_float>> matrix;
    Mode currentMode;
    int maxBuffers;
    int bufferCount;

    std::mt19937 rng;

    void generateRandom();
    void generateLinear();
    void generateMirrorPairs();

    std::vector<DSPSampleBuffer> buffersL;
    std::vector<DSPSampleBuffer> buffersR;

    std::vector<DSPSampleBuffer> tempBuffersL;
    std::vector<DSPSampleBuffer> tempBuffersR;
};
