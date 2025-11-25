#include "PolyphonicGenerator.h"
#include <algorithm>
#include <cmath>

namespace GuitarIO
{

    PolyphonicGenerator::PolyphonicGenerator(double sampleRate)
    {
        for (size_t i = 0; i < MAX_VOICES; ++i)
        {
            voices[i].SetSampleRate(sampleRate);
            voices[i].SetAmplitude(0.0f);
            frequencies[i] = 0.0;
        }
    }

    void PolyphonicGenerator::SetSampleRate(double sampleRate)
    {
        for (auto &voice : voices)
        {
            voice.SetSampleRate(sampleRate);
        }
    }

    void PolyphonicGenerator::SetVoiceFrequency(size_t voiceIndex, double frequency)
    {
        if (voiceIndex >= MAX_VOICES)
        {
            return;
        }

        frequencies[voiceIndex] = frequency;
        voices[voiceIndex].SetFrequency(frequency);

        if (frequency > 0.0)
        {
            voices[voiceIndex].SetAmplitude(1.0f);
        }
        else
        {
            voices[voiceIndex].SetAmplitude(0.0f);
        }

        UpdateActiveVoiceCount();
    }

    void PolyphonicGenerator::SetVoiceFrequencies(const std::array<float, MAX_VOICES> &freqs)
    {
        for (size_t i = 0; i < MAX_VOICES; ++i)
        {
            SetVoiceFrequency(i, static_cast<double>(freqs[i]));
        }
    }

    void PolyphonicGenerator::SetVoiceAmplitude(size_t voiceIndex, float amplitude)
    {
        if (voiceIndex >= MAX_VOICES)
        {
            return;
        }

        voices[voiceIndex].SetAmplitude(amplitude);
    }

    void PolyphonicGenerator::SetGlobalVolume(float volume)
    {
        globalVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    void PolyphonicGenerator::Generate(std::span<float> buffer, bool accumulate)
    {
        if (activeVoiceCount == 0)
        {
            if (!accumulate)
            {
                std::fill(buffer.begin(), buffer.end(), 0.0f);
            }
            return;
        }

        const float gainCompensation = 1.0f / std::sqrt(static_cast<float>(activeVoiceCount));
        const float effectiveVolume = globalVolume * gainCompensation;

        if (!accumulate)
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
        }

        for (size_t i = 0; i < MAX_VOICES; ++i)
        {
            if (frequencies[i] > 0.0)
            {
                float originalAmp = effectiveVolume;
                voices[i].SetAmplitude(originalAmp);
                voices[i].Generate(buffer, true);
            }
        }
    }

    void PolyphonicGenerator::Reset()
    {
        for (auto &voice : voices)
        {
            voice.Reset();
        }
    }

    size_t PolyphonicGenerator::GetActiveVoiceCount() const
    {
        return activeVoiceCount;
    }

    void PolyphonicGenerator::UpdateActiveVoiceCount()
    {
        activeVoiceCount = 0;
        for (double freq : frequencies)
        {
            if (freq > 0.0)
            {
                ++activeVoiceCount;
            }
        }
    }

} // namespace GuitarIO
