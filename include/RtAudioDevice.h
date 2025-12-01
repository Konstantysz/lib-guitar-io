#pragma once

#include "AudioDevice.h"
#include <memory>
#include <RtAudio.h>

namespace GuitarIO
{
    /**
     * @brief RtAudio-based implementation of AudioDevice interface
     *
     * Concrete implementation of the AudioDevice interface using RtAudio library.
     * Provides cross-platform audio I/O with native API support:
     * - Windows: ASIO (preferred) or WASAPI
     * - macOS: CoreAudio
     * - Linux: ALSA
     *
     * This class is the default production implementation. For testing,
     * use MockAudioDevice instead.
     */
    class RtAudioDevice : public AudioDevice
    {
    public:
        /**
         * @brief Constructs an RtAudio device instance
         */
        RtAudioDevice();

        /**
         * @brief Destructor
         */
        ~RtAudioDevice() override;

        RtAudioDevice(const RtAudioDevice &) = delete;

        RtAudioDevice &operator=(const RtAudioDevice &) = delete;

        /**
         * @brief Move constructor
         * @param other Instance to move from
         */
        RtAudioDevice(RtAudioDevice &&other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Instance to move from
         * @return Reference to this instance
         */
        RtAudioDevice &operator=(RtAudioDevice &&other) noexcept;

        /**
         * @brief Opens an audio stream
         * @param deviceId Device ID (use default if not specified)
         * @param config Stream configuration
         * @param userCallback Audio processing callback
         * @param userPtr User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool Open(uint32_t deviceId,
            const AudioStreamConfig &config,
            AudioCallback userCallback,
            void *userPtr = nullptr) override;

        /**
         * @brief Opens the default audio input device
         * @param config Stream configuration
         * @param userCallback Audio processing callback
         * @param userPtr User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool OpenDefault(const AudioStreamConfig &config, AudioCallback userCallback, void *userPtr = nullptr) override;

        /**
         * @brief Starts the audio stream
         * @return true on success, false on failure
         */
        bool Start() override;

        /**
         * @brief Stops the audio stream
         * @return true on success, false on failure
         */
        bool Stop() override;

        /**
         * @brief Closes the audio stream
         */
        void Close() override;

        /**
         * @brief Checks if stream is open
         * @return true if open, false otherwise
         */
        [[nodiscard]] bool IsOpen() const override;

        /**
         * @brief Checks if stream is running
         * @return true if running, false otherwise
         */
        [[nodiscard]] bool IsRunning() const override;

        /**
         * @brief Returns the last error message
         * @return Error message string
         */
        [[nodiscard]] std::string GetLastError() const override;

    private:
        /**
         * @brief RtAudio callback function
         * @param outputBuffer Output audio buffer
         * @param inputBuffer Input audio buffer
         * @param nFrames Number of frames
         * @param streamTime Stream time
         * @param status Stream status
         * @param userData User data pointer
         * @return 0 to continue, non-zero to stop stream
         */
        static int RtAudioCallback(void *outputBuffer,
            void *inputBuffer,
            unsigned int nFrames,
            double streamTime,
            RtAudioStreamStatus status,
            void *userData);

        mutable RtAudio rtAudio;                ///< RtAudio instance
        AudioCallback callback;                 ///< User callback function
        void *userData = nullptr;               ///< User data pointer
        mutable std::string lastError;          ///< Last error message
        RtAudio::StreamParameters inputParams;  ///< Input stream parameters
        RtAudio::StreamParameters outputParams; ///< Output stream parameters
        bool hasInput = false;                  ///< Flag indicating input is enabled
        bool hasOutput = false;                 ///< Flag indicating output is enabled
    };

} // namespace GuitarIO
