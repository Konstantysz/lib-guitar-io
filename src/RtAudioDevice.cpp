#include "RtAudioDevice.h"
#include <stdexcept>
#include <RtAudio.h>

namespace GuitarIO
{
    RtAudioDevice::RtAudioDevice()
    {
    }

    RtAudioDevice::~RtAudioDevice()
    {
        Close();
    }

    bool RtAudioDevice::Open(uint32_t deviceId,
        const AudioStreamConfig &config,
        AudioCallback userCallback,
        void *userPtr)
    {
        if (IsOpen())
        {
            lastError = "Device already open";
            return false;
        }

        this->callback = std::move(userCallback);
        this->userData = userPtr;

        // Configure input parameters
        if (config.inputChannels > 0)
        {
            inputParams.deviceId = deviceId;
            inputParams.nChannels = config.inputChannels;
            inputParams.firstChannel = 0;
            hasInput = true;
        }

        // Configure output parameters (if needed)
        if (config.outputChannels > 0)
        {
            outputParams.deviceId = deviceId;
            outputParams.nChannels = config.outputChannels;
            outputParams.firstChannel = 0;
            hasOutput = true;
        }

        // Open stream
        unsigned int bufferFrames = config.bufferSize;
        unsigned int sampleRate = config.sampleRate;

        RtAudioErrorType result = rtAudio.openStream(hasOutput ? &outputParams : nullptr,
            hasInput ? &inputParams : nullptr,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &RtAudioDevice::RtAudioCallback,
            this);

        if (result != RTAUDIO_NO_ERROR)
        {
            lastError = rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    bool RtAudioDevice::OpenDefault(const AudioStreamConfig &config, AudioCallback userCallback, void *userPtr)
    {
        uint32_t defaultDevice = rtAudio.getDefaultInputDevice();
        return Open(defaultDevice, config, std::move(userCallback), userPtr);
    }

    bool RtAudioDevice::Start()
    {
        if (!IsOpen())
        {
            lastError = "Device not open";
            return false;
        }

        RtAudioErrorType result = rtAudio.startStream();
        if (result != RTAUDIO_NO_ERROR)
        {
            lastError = rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    bool RtAudioDevice::Stop()
    {
        if (!IsRunning())
        {
            lastError = "Stream not running";
            return false;
        }

        RtAudioErrorType result = rtAudio.stopStream();
        if (result != RTAUDIO_NO_ERROR)
        {
            lastError = rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    void RtAudioDevice::Close()
    {
        if (IsOpen())
        {
            if (IsRunning())
            {
                rtAudio.stopStream();
            }
            rtAudio.closeStream();
        }

        hasInput = false;
        hasOutput = false;
    }

    bool RtAudioDevice::IsOpen() const
    {
        return rtAudio.isStreamOpen();
    }

    bool RtAudioDevice::IsRunning() const
    {
        return rtAudio.isStreamRunning();
    }

    std::string RtAudioDevice::GetLastError() const
    {
        return lastError;
    }

    int RtAudioDevice::RtAudioCallback(void *outputBuffer,
        void *inputBuffer,
        unsigned int nFrames,
        double /*streamTime*/,
        RtAudioStreamStatus /*status*/,
        void *userData)
    {
        auto *device = static_cast<RtAudioDevice *>(userData);
        if (!device || !device->callback)
        {
            return 1; // Stop stream
        }

        // Create std::span wrappers for buffers
        std::span<const float> inputSpan;
        std::span<float> outputSpan;

        if (inputBuffer != nullptr)
        {
            unsigned int channels = device->hasInput ? device->inputParams.nChannels : 1;
            inputSpan = std::span<const float>(static_cast<const float *>(inputBuffer), nFrames * channels);
        }

        if (outputBuffer != nullptr)
        {
            unsigned int channels = device->hasOutput ? device->outputParams.nChannels : 1;
            outputSpan = std::span<float>(static_cast<float *>(outputBuffer), nFrames * channels);
        }

        return device->callback(inputSpan, outputSpan, device->userData);
    }

} // namespace GuitarIO
