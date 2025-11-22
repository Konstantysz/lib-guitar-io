#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

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
     * @param inputBuffer Input audio buffer (nullptr if no input)
     * @param outputBuffer Output audio buffer (nullptr if no output)
     * @param frameCount Number of frames in buffer
     * @param userData User data pointer
     * @return 0 to continue, non-zero to stop stream
     */
    using AudioCallback =
        std::function<int(const float *inputBuffer, float *outputBuffer, size_t frameCount, void *userData)>;

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

        // Non-copyable
        AudioDevice(const AudioDevice &) = delete;
        AudioDevice &operator=(const AudioDevice &) = delete;

        // Movable
        AudioDevice(AudioDevice &&) noexcept;
        AudioDevice &operator=(AudioDevice &&) noexcept;

        /**
         * @brief Opens an audio stream
         * @param deviceId Device ID (use default if not specified)
         * @param config Stream configuration
         * @param callback Audio processing callback
         * @param userData User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool Open(uint32_t deviceId, const AudioStreamConfig &config, AudioCallback callback, void *userData = nullptr);

        /**
         * @brief Opens the default audio input device
         * @param config Stream configuration
         * @param callback Audio processing callback
         * @param userData User data pointer passed to callback
         * @return true on success, false on failure
         */
        bool OpenDefault(const AudioStreamConfig &config, AudioCallback callback, void *userData = nullptr);

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
        class Impl;
        std::unique_ptr<Impl> pImpl; ///< PIMPL for RtAudio encapsulation
    };

} // namespace GuitarIO
