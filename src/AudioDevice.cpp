#include "AudioDevice.h"
#include <RtAudio.h>
#include <stdexcept>

namespace GuitarIO
{
    /**
     * @brief PIMPL implementation class wrapping RtAudio
     */
    class AudioDevice::Impl
    {
    public:
        RtAudio rtAudio;
        AudioCallback callback;
        void* userData = nullptr;
        std::string lastError;
        RtAudio::StreamParameters inputParams;
        RtAudio::StreamParameters outputParams;
        bool hasInput = false;
        bool hasOutput = false;

        /**
         * @brief RtAudio callback adapter
         */
        static int RtAudioCallback(void* outputBuffer, void* inputBuffer,
                                   unsigned int nFrames,
                                   double /*streamTime*/,
                                   RtAudioStreamStatus /*status*/,
                                   void* userData)
        {
            auto* impl = static_cast<Impl*>(userData);
            if (!impl || !impl->callback)
            {
                return 1; // Stop stream
            }

            return impl->callback(
                static_cast<const float*>(inputBuffer),
                static_cast<float*>(outputBuffer),
                static_cast<size_t>(nFrames),
                impl->userData
            );
        }
    };

    AudioDevice::AudioDevice()
        : pImpl(std::make_unique<Impl>())
    {
    }

    AudioDevice::~AudioDevice()
    {
        Close();
    }

    AudioDevice::AudioDevice(AudioDevice&&) noexcept = default;
    AudioDevice& AudioDevice::operator=(AudioDevice&&) noexcept = default;

    bool AudioDevice::Open(uint32_t deviceId, const AudioStreamConfig& config,
                           AudioCallback callback, void* userData)
    {
        if (IsOpen())
        {
            pImpl->lastError = "Device already open";
            return false;
        }

        pImpl->callback = std::move(callback);
        pImpl->userData = userData;

        try
        {
            // Configure input parameters
            if (config.inputChannels > 0)
            {
                pImpl->inputParams.deviceId = deviceId;
                pImpl->inputParams.nChannels = config.inputChannels;
                pImpl->inputParams.firstChannel = 0;
                pImpl->hasInput = true;
            }

            // Configure output parameters (if needed)
            if (config.outputChannels > 0)
            {
                pImpl->outputParams.deviceId = deviceId;
                pImpl->outputParams.nChannels = config.outputChannels;
                pImpl->outputParams.firstChannel = 0;
                pImpl->hasOutput = true;
            }

            // Open stream
            unsigned int bufferFrames = config.bufferSize;
            unsigned int sampleRate = config.sampleRate;

            pImpl->rtAudio.openStream(
                pImpl->hasOutput ? &pImpl->outputParams : nullptr,
                pImpl->hasInput ? &pImpl->inputParams : nullptr,
                RTAUDIO_FLOAT32,
                sampleRate,
                &bufferFrames,
                &Impl::RtAudioCallback,
                pImpl.get()
            );

            return true;
        }
        catch (const RtAudioError& error)
        {
            pImpl->lastError = error.getMessage();
            return false;
        }
    }

    bool AudioDevice::OpenDefault(const AudioStreamConfig& config,
                                   AudioCallback callback, void* userData)
    {
        try
        {
            uint32_t defaultDevice = pImpl->rtAudio.getDefaultInputDevice();
            return Open(defaultDevice, config, std::move(callback), userData);
        }
        catch (const RtAudioError& error)
        {
            pImpl->lastError = error.getMessage();
            return false;
        }
    }

    bool AudioDevice::Start()
    {
        if (!IsOpen())
        {
            pImpl->lastError = "Device not open";
            return false;
        }

        try
        {
            pImpl->rtAudio.startStream();
            return true;
        }
        catch (const RtAudioError& error)
        {
            pImpl->lastError = error.getMessage();
            return false;
        }
    }

    bool AudioDevice::Stop()
    {
        if (!IsRunning())
        {
            pImpl->lastError = "Stream not running";
            return false;
        }

        try
        {
            pImpl->rtAudio.stopStream();
            return true;
        }
        catch (const RtAudioError& error)
        {
            pImpl->lastError = error.getMessage();
            return false;
        }
    }

    void AudioDevice::Close()
    {
        if (IsOpen())
        {
            try
            {
                if (IsRunning())
                {
                    pImpl->rtAudio.stopStream();
                }
                pImpl->rtAudio.closeStream();
            }
            catch (const RtAudioError&)
            {
                // Ignore errors during cleanup
            }
        }

        pImpl->hasInput = false;
        pImpl->hasOutput = false;
    }

    bool AudioDevice::IsOpen() const
    {
        return pImpl->rtAudio.isStreamOpen();
    }

    bool AudioDevice::IsRunning() const
    {
        return pImpl->rtAudio.isStreamRunning();
    }

    std::string AudioDevice::GetLastError() const
    {
        return pImpl->lastError;
    }

} // namespace GuitarIO
