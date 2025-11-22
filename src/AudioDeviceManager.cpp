#include "AudioDeviceManager.h"
#include <RtAudio.h>

namespace GuitarIO
{
    AudioDeviceManager &AudioDeviceManager::Get()
    {
        static AudioDeviceManager instance;
        return instance;
    }

    std::vector<AudioDeviceInfo> AudioDeviceManager::EnumerateInputDevices() const
    {
        std::vector<AudioDeviceInfo> devices;

        RtAudio audio;
        const auto deviceCount = audio.getDeviceCount();

        for (unsigned int i = 0; i < deviceCount; ++i)
        {
            RtAudio::DeviceInfo info = audio.getDeviceInfo(i);

            // Only include devices with input channels
            if (info.inputChannels > 0)
            {
                AudioDeviceInfo deviceInfo;
                deviceInfo.name = info.name;
                deviceInfo.id = i;
                deviceInfo.maxInputChannels = info.inputChannels;
                deviceInfo.maxOutputChannels = info.outputChannels;
                deviceInfo.supportedSampleRates = info.sampleRates;

                devices.push_back(deviceInfo);
            }
        }

        return devices;
    }

    uint32_t AudioDeviceManager::GetDefaultInputDevice() const
    {
        RtAudio audio;
        return audio.getDefaultInputDevice();
    }

    AudioDeviceInfo AudioDeviceManager::GetDeviceInfo(uint32_t deviceId) const
    {
        RtAudio audio;
        RtAudio::DeviceInfo info = audio.getDeviceInfo(deviceId);

        AudioDeviceInfo deviceInfo;
        deviceInfo.name = info.name;
        deviceInfo.id = deviceId;
        deviceInfo.maxInputChannels = info.inputChannels;
        deviceInfo.maxOutputChannels = info.outputChannels;
        deviceInfo.supportedSampleRates = info.sampleRates;

        return deviceInfo;
    }

} // namespace GuitarIO
