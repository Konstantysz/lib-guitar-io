#pragma once

#include "AudioDevice.h"
#include <RtAudio.h>
#include <vector>

namespace GuitarIO
{
    /**
     * @brief Audio device enumeration and management
     *
     * Singleton class that provides device enumeration using a shared RtAudio instance.
     * This avoids conflicts when an audio stream is already active.
     *
     * IMPORTANT: Audio device IDs are platform-specific and not sequential.
     * On Windows, IDs may be values like 132, 133, etc., not 0, 1, 2...
     * Always use getDeviceIds() to iterate over valid device IDs.
     */
    class AudioDeviceManager
    {
    public:
        static AudioDeviceManager &Get();

        /**
         * @brief Enumerate all available audio input devices
         * @return Vector of device information for devices with input channels
         */
        [[nodiscard]] std::vector<AudioDeviceInfo> EnumerateInputDevices() const;

        /**
         * @brief Get default input device ID
         * @return Platform default input device ID
         */
        [[nodiscard]] uint32_t GetDefaultInputDevice() const;

        /**
         * @brief Get device information by ID
         * @param deviceId Platform-specific device ID
         * @return Device information (empty name if device not found)
         */
        [[nodiscard]] AudioDeviceInfo GetDeviceInfo(uint32_t deviceId) const;

    private:
        AudioDeviceManager() = default;
        ~AudioDeviceManager() = default;

        AudioDeviceManager(const AudioDeviceManager &) = delete;
        AudioDeviceManager &operator=(const AudioDeviceManager &) = delete;
        AudioDeviceManager(AudioDeviceManager &&) = delete;
        AudioDeviceManager &operator=(AudioDeviceManager &&) = delete;

        mutable RtAudio rtAudio;  ///< Shared instance for device queries
    };

} // namespace GuitarIO
