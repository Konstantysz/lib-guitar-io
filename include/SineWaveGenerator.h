#pragma once

#include <span>

namespace GuitarIO
{
    /**
     * @brief Simple sine wave generator for audio synthesis
     */
    class SineWaveGenerator
    {
    public:
        /**
         * @brief Constructs a sine wave generator
         * @param sampleRate Audio sample rate in Hz
         */
        explicit SineWaveGenerator(double sampleRate = 48000.0);

        /**
         * @brief Sets the frequency of the sine wave
         * @param freq Frequency in Hz
         */
        void SetFrequency(double freq);

        /**
         * @brief Sets the amplitude (volume)
         * @param amp Volume level (0.0 to 1.0)
         */
        void SetAmplitude(float amp);

        /**
         * @brief Sets the sample rate
         * @param rate Sample rate in Hz
         */
        void SetSampleRate(double rate);

        /**
         * @brief Generates sine wave samples into the output buffer
         * @param buffer Output buffer to fill
         * @param accumulate If true, adds to existing buffer content instead of overwriting
         */
        void Generate(std::span<float> buffer, bool accumulate = false);

        /**
         * @brief Resets the phase to 0
         */
        void Reset();

    private:
        /**
         * @brief Updates the phase increment based on frequency and sample rate
         */
        void UpdateIncrement();

        double sampleRate = 48000.0; ///< Audio sample rate in Hz
        double frequency = 440.0;    ///< Wave frequency in Hz
        float amplitude = 0.5f;      ///< Wave amplitude [0.0, 1.0]
        double currentPhase = 0.0;   ///< Current phase accumulator [0.0, 2*PI]
        double phaseIncrement = 0.0; ///< Phase increment per sample
    };
} // namespace GuitarIO
