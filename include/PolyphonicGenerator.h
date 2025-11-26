#pragma once

#include "SineWaveGenerator.h"
#include <array>
#include <span>

namespace GuitarIO
{
    /**
     * @brief Polyphonic tone generator for simultaneous multi-frequency playback
     *
     * Useful for playing reference chords when tuning in polyphonic mode.
     * Supports up to 6 simultaneous tones (for guitar strings).
     */
    class PolyphonicGenerator
    {
    public:
        static constexpr size_t MAX_VOICES = 6; ///< Maximum number of simultaneous tones

        /**
         * @brief Constructs a polyphonic generator
         * @param sampleRate Audio sample rate in Hz
         */
        explicit PolyphonicGenerator(double sampleRate = 48000.0);

        /**
         * @brief Sets the sample rate for all oscillators
         * @param sampleRate Sample rate in Hz
         */
        void SetSampleRate(double sampleRate);

        /**
         * @brief Sets the frequency for a specific voice
         * @param voiceIndex Voice index (0-5)
         * @param frequency Frequency in Hz (0 = disabled)
         */
        void SetVoiceFrequency(size_t voiceIndex, double frequency);

        /**
         * @brief Sets frequencies for all voices from an array
         * @param frequencies Array of 6 frequencies (0 = disabled)
         */
        void SetVoiceFrequencies(const std::array<float, MAX_VOICES> &frequencies);

        /**
         * @brief Sets the amplitude for a specific voice
         * @param voiceIndex Voice index (0-5)
         * @param amplitude Volume level (0.0 to 1.0)
         */
        void SetVoiceAmplitude(size_t voiceIndex, float amplitude);

        /**
         * @brief Sets the global volume level
         * @param volume Volume level (0.0 to 1.0)
         */
        void SetGlobalVolume(float volume);

        /**
         * @brief Generates polyphonic samples into the output buffer
         * @param buffer Output buffer to fill
         * @param accumulate If true, adds to existing buffer content instead of overwriting
         */
        void Generate(std::span<float> buffer, bool accumulate = false);

        /**
         * @brief Resets all voice phases to 0
         */
        void Reset();

        /**
         * @brief Gets the number of active voices
         */
        [[nodiscard]] size_t GetActiveVoiceCount() const;

    private:
        std::array<SineWaveGenerator, MAX_VOICES> voices; ///< Array of oscillators
        std::array<double, MAX_VOICES> frequencies;       ///< Array of frequencies
        float globalVolume = 0.5f;                        ///< Global volume
        size_t activeVoiceCount = 0;                      ///< Number of active voices

        void UpdateActiveVoiceCount();
    };

} // namespace GuitarIO
