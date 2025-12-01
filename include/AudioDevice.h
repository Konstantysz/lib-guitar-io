#pragma once

#include <cstdint>
#include <functional>
#include <span>
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
     * @param inputBuffer Input audio buffer (empty span if no input)
     * @param outputBuffer Output audio buffer (empty span if no output)
     * @param userData User data pointer
     * @return 0 to continue, non-zero to stop stream
     */
    using AudioCallback =
        std::function<int(std::span<const float> inputBuffer, std::span<float> outputBuffer, void *userData)>;

    /**
     * @brief Interface for audio devices
     *
     * Defines the contract for audio I/O operations, allowing for
     * concrete implementations (RtAudio) and mocks (for testing).
     */
    class AudioDevice
    {
    public:
        virtual ~AudioDevice() = default;

        /**
         * @brief Opens an audio stream
         * @param deviceId Device ID (use default if not specified)
         * @param config Stream configuration
         * @param userCallback Audio processing callback
         * @param userPtr User data pointer passed to callback
         * @return true on success, false on failure
         */
        virtual bool Open(uint32_t deviceId,
            const AudioStreamConfig &config,
            AudioCallback userCallback,
            void *userPtr = nullptr) = 0;

        /**
         * @brief Opens the default audio input device
         * @param config Stream configuration
         * @param userCallback Audio processing callback
         * @param userPtr User data pointer passed to callback
         * @return true on success, false on failure
         */
        virtual bool
            OpenDefault(const AudioStreamConfig &config, AudioCallback userCallback, void *userPtr = nullptr) = 0;

        /**
         * @brief Starts the audio stream
         * @return true on success, false on failure
         */
        virtual bool Start() = 0;

        /**
         * @brief Stops the audio stream
         * @return true on success, false on failure
         */
        virtual bool Stop() = 0;

        /**
         * @brief Closes the audio stream
         */
        virtual void Close() = 0;

        /**
         * @brief Checks if stream is open
         * @return true if open, false otherwise
         */
        [[nodiscard]] virtual bool IsOpen() const = 0;

        /**
         * @brief Checks if stream is running
         * @return true if running, false otherwise
         */
        [[nodiscard]] virtual bool IsRunning() const = 0;

        /**
         * @brief Returns the last error message
         * @return Error message string
         */
        [[nodiscard]] virtual std::string GetLastError() const = 0;
    };

} // namespace GuitarIO
