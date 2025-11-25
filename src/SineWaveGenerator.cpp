#include "SineWaveGenerator.h"
#include <cmath>
#include <numbers>

namespace GuitarIO
{

    SineWaveGenerator::SineWaveGenerator(double sampleRate) : sampleRate(sampleRate)
    {
    }

    void SineWaveGenerator::SetFrequency(double freq)
    {
        this->frequency = freq;
        UpdateIncrement();
    }

    void SineWaveGenerator::SetAmplitude(float amp)
    {
        this->amplitude = amp;
    }

    void SineWaveGenerator::SetSampleRate(double rate)
    {
        this->sampleRate = rate;
        UpdateIncrement();
    }

    void SineWaveGenerator::Generate(std::span<float> buffer, bool accumulate)
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

    void SineWaveGenerator::Reset()
    {
        currentPhase = 0.0;
    }

    void SineWaveGenerator::UpdateIncrement()
    {
        phaseIncrement = (2.0 * std::numbers::pi * frequency) / sampleRate;
    }

} // namespace GuitarIO
