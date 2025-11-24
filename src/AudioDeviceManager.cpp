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

        std::vector<unsigned int> deviceIds = rtAudio.getDeviceIds();

        for (unsigned int deviceId : deviceIds)
        {
            RtAudio::DeviceInfo info = rtAudio.getDeviceInfo(deviceId);

            if (info.inputChannels > 0)
            {
                AudioDeviceInfo deviceInfo;
                deviceInfo.name = info.name;
                deviceInfo.id = deviceId;
                deviceInfo.maxInputChannels = info.inputChannels;
                deviceInfo.maxOutputChannels = info.outputChannels;
                deviceInfo.supportedSampleRates = info.sampleRates;

                devices.push_back(deviceInfo);
            }
        }

        return devices;
    }

    std::vector<AudioDeviceInfo> AudioDeviceManager::EnumerateOutputDevices() const
    {
        std::vector<AudioDeviceInfo> devices;

        std::vector<unsigned int> deviceIds = rtAudio.getDeviceIds();

        for (unsigned int deviceId : deviceIds)
        {
            RtAudio::DeviceInfo info = rtAudio.getDeviceInfo(deviceId);

            if (info.outputChannels > 0)
            {
                AudioDeviceInfo deviceInfo;
                deviceInfo.name = info.name;
                deviceInfo.id = deviceId;
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
        return rtAudio.getDefaultInputDevice();
    }

    uint32_t AudioDeviceManager::GetDefaultOutputDevice() const
    {
        return rtAudio.getDefaultOutputDevice();
    }

    AudioDeviceInfo AudioDeviceManager::GetDeviceInfo(uint32_t deviceId) const
    {
        RtAudio::DeviceInfo info = rtAudio.getDeviceInfo(deviceId);

        AudioDeviceInfo deviceInfo;
        deviceInfo.name = info.name;
        deviceInfo.id = deviceId;
        deviceInfo.maxInputChannels = info.inputChannels;
        deviceInfo.maxOutputChannels = info.outputChannels;
        deviceInfo.supportedSampleRates = info.sampleRates;

        return deviceInfo;
    }

} // namespace GuitarIO
