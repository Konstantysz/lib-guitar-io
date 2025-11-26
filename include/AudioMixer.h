#pragma once

#include <algorithm>
#include <span>
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
        static void Mix(std::span<const float> input, std::span<float> output, float gain);

        /**
         * @brief Clears the buffer (fills with silence)
         * @param buffer Buffer to clear
         */
        static void Clear(std::span<float> buffer);

        /**
         * @brief Applies hard clipping limiter to prevent clipping
         * @param buffer Buffer to limit
         * @param threshold Threshold level (usually 1.0)
         */
        static void Limit(std::span<float> buffer, float threshold = 1.0f);
    };
} // namespace GuitarIO
