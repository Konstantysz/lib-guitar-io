#include "AudioMixer.h"

namespace GuitarIO
{
    void AudioMixer::Mix(std::span<const float> input, std::span<float> output, float gain)
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

    void AudioMixer::Clear(std::span<float> buffer)
    {
        std::ranges::fill(buffer, 0.0f);
    }

    void AudioMixer::Limit(std::span<float> buffer, float threshold)
    {
        for (float &sample : buffer)
        {
            sample = std::clamp(sample, -threshold, threshold);
        }
    }
} // namespace GuitarIO
