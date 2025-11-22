#pragma once

#include "AudioDevice.h"
#include <vector>

namespace GuitarIO
{
    /**
     * @brief Audio device enumeration and management
     */
    class AudioDeviceManager
    {
    public:
        /**
         * @brief Returns the singleton instance
         * @return AudioDeviceManager instance
         */
        static AudioDeviceManager& Get();

        /**
         * @brief Enumerates all available audio input devices
         * @return Vector of device information
         */
        [[nodiscard]] std::vector<AudioDeviceInfo> EnumerateInputDevices() const;

        /**
         * @brief Returns the default input device ID
         * @return Default device ID
         */
        [[nodiscard]] uint32_t GetDefaultInputDevice() const;

        /**
         * @brief Returns device information by ID
         * @param deviceId Device ID
         * @return Device information (empty name if not found)
         */
        [[nodiscard]] AudioDeviceInfo GetDeviceInfo(uint32_t deviceId) const;

    private:
        AudioDeviceManager() = default;
        ~AudioDeviceManager() = default;

        // Non-copyable, non-movable
        AudioDeviceManager(const AudioDeviceManager&) = delete;
        AudioDeviceManager& operator=(const AudioDeviceManager&) = delete;
        AudioDeviceManager(AudioDeviceManager&&) = delete;
        AudioDeviceManager& operator=(AudioDeviceManager&&) = delete;
    };

} // namespace GuitarIO
