#pragma once

#include <span>
#include <cmath>
#include <numbers>

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
        explicit SineWaveGenerator(double sampleRate = 48000.0)
            : sampleRate(sampleRate)
        {
        }

        /**
         * @brief Sets the frequency of the sine wave
         * @param frequency Frequency in Hz
         */
        void SetFrequency(double frequency)
        {
            this->frequency = frequency;
            UpdateIncrement();
        }

        /**
         * @brief Sets the amplitude (volume)
         * @param amplitude Volume level (0.0 to 1.0)
         */
        void SetAmplitude(float amplitude)
        {
            this->amplitude = amplitude;
        }

        /**
         * @brief Sets the sample rate
         * @param sampleRate Sample rate in Hz
         */
        void SetSampleRate(double sampleRate)
        {
            this->sampleRate = sampleRate;
            UpdateIncrement();
        }

        /**
         * @brief Generates sine wave samples into the output buffer
         * @param buffer Output buffer to fill
         * @param accumulate If true, adds to existing buffer content instead of overwriting
         */
        void Generate(std::span<float> buffer, bool accumulate = false)
        {
            for (float &sample : buffer)
            {
                float value = amplitude * std::sin(static_cast<float>(currentPhase));
                
                if (accumulate)
                {
                    sample += value;
                }
                else
                {
                    sample = value;
                }

                currentPhase += phaseIncrement;
                if (currentPhase >= 2.0 * std::numbers::pi)
                {
                    currentPhase -= 2.0 * std::numbers::pi;
                }
            }
        }

        /**
         * @brief Resets the phase to 0
         */
        void Reset()
        {
            currentPhase = 0.0;
        }

    private:
        void UpdateIncrement()
        {
            phaseIncrement = (2.0 * std::numbers::pi * frequency) / sampleRate;
        }

        double sampleRate = 48000.0;
        double frequency = 440.0;
        float amplitude = 0.5f;
        double currentPhase = 0.0;
        double phaseIncrement = 0.0;
    };
}
