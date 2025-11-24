#pragma once

#include <span>
#include <algorithm>
#include <vector>

namespace GuitarIO
{
    /**
     * @brief Simple audio mixer for combining signals
     */
    class AudioMixer
    {
    public:
        /**
         * @brief Mixes input buffer into output buffer with gain control
         * @param input Input audio buffer
         * @param output Output audio buffer (accumulates result)
         * @param gain Volume multiplier for input signal
         */
        static void Mix(std::span<const float> input, std::span<float> output, float gain)
        {
            if (input.empty() || output.empty() || input.size() != output.size())
            {
                return;
            }

            for (size_t i = 0; i < output.size(); ++i)
            {
                output[i] += input[i] * gain;
            }
        }

        /**
         * @brief Clears the buffer (fills with silence)
         * @param buffer Buffer to clear
         */
        static void Clear(std::span<float> buffer)
        {
            std::ranges::fill(buffer, 0.0f);
        }
        
        /**
         * @brief Applies hard clipping limiter to prevent clipping
         * @param buffer Buffer to limit
         * @param threshold Threshold level (usually 1.0)
         */
        static void Limit(std::span<float> buffer, float threshold = 1.0f)
        {
            for (float &sample : buffer)
            {
                sample = std::clamp(sample, -threshold, threshold);
            }
        }
    };
}
