#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include <RtAudio.h>

namespace GuitarIO
{
    /**
     * @brief Audio device information
     */
    struct AudioDeviceInfo
    {
        std::string name;                           ///< Device name
        uint32_t id;                                ///< Device ID
        uint32_t maxInputChannels;                  ///< Maximum input channels
        uint32_t maxOutputChannels;                 ///< Maximum output channels
        std::vector<uint32_t> supportedSampleRates; ///< Supported sample rates
    };

    /**
     * @brief Audio stream configuration
     */
    struct AudioStreamConfig
    {
        uint32_t sampleRate = 48000; ///< Sample rate (Hz)
        uint32_t bufferSize = 512;   ///< Buffer size (frames)
        uint32_t inputChannels = 1;  ///< Number of input channels
        uint32_t outputChannels = 0; ///< Number of output channels (0 for input-only)
    };

    /**
     * @brief Audio callback function type
     * @param inputBuffer Input audio buffer (empty span if no input)
     * @param outputBuffer Output audio buffer (empty span if no output)
     * @param userData User data pointer
     * @return 0 to continue, non-zero to stop stream
     */
    using AudioCallback =
        std::function<int(std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData)>;

    /**
     * @brief Cross-platform audio device abstraction
     *
     * Wraps RtAudio to provide a clean, platform-agnostic interface for audio I/O.
     * Uses ASIO on Windows, CoreAudio on macOS, and ALSA on Linux.
     */
    class AudioDevice
    {
    public:
        /**
         * @brief Constructs an audio device
         */
        AudioDevice();

        /**
         * @brief Destructor
         */
        ~AudioDevice();

        AudioDevice(const AudioDevice &) = delete;

        AudioDevice &operator=(const AudioDevice &) = delete;

        /**
         * @brief Move constructor
         * @param other Instance to move from
         */
        AudioDevice(AudioDevice &&other) noexcept;

        /**
         * @brief Move assignment operator
         * @param other Instance to move from
         * @return Reference to this instance
         */
        AudioDevice &operator=(AudioDevice &&other) noexcept;

        /**
         * @brief Opens an audio stream
         * @param deviceId Device ID (use default if not specified)
         * @param config Stream configuration
         * @param callback Audio processing callback
         * @param userData User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool Open(uint32_t deviceId,
            const AudioStreamConfig &config,
            AudioCallback userCallback,
            void *userPtr = nullptr);

        /**
         * @brief Opens the default audio input device
         * @param config Stream configuration
         * @param userCallback Audio processing callback
         * @param userPtr User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool OpenDefault(const AudioStreamConfig &config, AudioCallback userCallback, void *userPtr = nullptr);

        /**
         * @brief Starts the audio stream
         * @return true on success, false on failure
         */
        bool Start();

        /**
         * @brief Stops the audio stream
         * @return true on success, false on failure
         */
        bool Stop();

        /**
         * @brief Closes the audio stream
         */
        void Close();

        /**
         * @brief Checks if stream is open
         * @return true if open, false otherwise
         */
        [[nodiscard]] bool IsOpen() const;

        /**
         * @brief Checks if stream is running
         * @return true if running, false otherwise
         */
        [[nodiscard]] bool IsRunning() const;

        /**
         * @brief Returns the last error message
         * @return Error message string
         */
        [[nodiscard]] std::string GetLastError() const;

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
