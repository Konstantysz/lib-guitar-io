#include "AudioDevice.h"
#include <stdexcept>
#include <RtAudio.h>

namespace GuitarIO
{
    /**
     * @brief PIMPL implementation class wrapping RtAudio
     */
    class AudioDevice::Impl
    {
    public:
        RtAudio rtAudio;                        ///< RtAudio instance
        AudioCallback callback;                 ///< User callback function
        void *userData = nullptr;               ///< User data pointer
        std::string lastError;                  ///< Last error message
        RtAudio::StreamParameters inputParams;  ///< Input stream parameters
        RtAudio::StreamParameters outputParams; ///< Output stream parameters
        bool hasInput = false;                  ///< Flag indicating input is enabled
        bool hasOutput = false;                 ///< Flag indicating output is enabled

        /**
         * @brief RtAudio callback adapter
         */
        static int RtAudioCallback(void *outputBuffer,
            void *inputBuffer,
            unsigned int nFrames,
            double /*streamTime*/,
            RtAudioStreamStatus /*status*/,
            void *userData)
        {
            auto *impl = static_cast<Impl *>(userData);
            if (!impl || !impl->callback)
            {
                return 1; // Stop stream
            }

            // Create std::span wrappers for buffers
            std::span<const float> inputSpan;
            std::span<float> outputSpan;

            if (inputBuffer != nullptr)
            {
                unsigned int channels = impl->hasInput ? impl->inputParams.nChannels : 1;
                inputSpan = std::span<const float>(static_cast<const float *>(inputBuffer), nFrames * channels);
            }

            if (outputBuffer != nullptr)
            {
                unsigned int channels = impl->hasOutput ? impl->outputParams.nChannels : 1;
                outputSpan = std::span<float>(static_cast<float *>(outputBuffer), nFrames * channels);
            }

            return impl->callback(inputSpan, outputSpan, impl->userData);
        }
    };

    AudioDevice::AudioDevice() : pImpl(std::make_unique<Impl>())
    {
    }

    AudioDevice::~AudioDevice()
    {
        Close();
    }

    AudioDevice::AudioDevice(AudioDevice &&) noexcept = default;
    AudioDevice &AudioDevice::operator=(AudioDevice &&) noexcept = default;

    bool AudioDevice::Open(uint32_t deviceId, const AudioStreamConfig &config, AudioCallback callback, void *userData)
    {
        if (IsOpen())
        {
            pImpl->lastError = "Device already open";
            return false;
        }

        pImpl->callback = std::move(callback);
        pImpl->userData = userData;

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

        RtAudioErrorType result = pImpl->rtAudio.openStream(pImpl->hasOutput ? &pImpl->outputParams : nullptr,
            pImpl->hasInput ? &pImpl->inputParams : nullptr,
            RTAUDIO_FLOAT32,
            sampleRate,
            &bufferFrames,
            &Impl::RtAudioCallback,
            pImpl.get());

        if (result != RTAUDIO_NO_ERROR)
        {
            pImpl->lastError = pImpl->rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    bool AudioDevice::OpenDefault(const AudioStreamConfig &config, AudioCallback callback, void *userData)
    {
        uint32_t defaultDevice = pImpl->rtAudio.getDefaultInputDevice();
        return Open(defaultDevice, config, std::move(callback), userData);
    }

    bool AudioDevice::Start()
    {
        if (!IsOpen())
        {
            pImpl->lastError = "Device not open";
            return false;
        }

        RtAudioErrorType result = pImpl->rtAudio.startStream();
        if (result != RTAUDIO_NO_ERROR)
        {
            pImpl->lastError = pImpl->rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    bool AudioDevice::Stop()
    {
        if (!IsRunning())
        {
            pImpl->lastError = "Stream not running";
            return false;
        }

        RtAudioErrorType result = pImpl->rtAudio.stopStream();
        if (result != RTAUDIO_NO_ERROR)
        {
            pImpl->lastError = pImpl->rtAudio.getErrorText();
            return false;
        }

        return true;
    }

    void AudioDevice::Close()
    {
        if (IsOpen())
        {
            if (IsRunning())
            {
                pImpl->rtAudio.stopStream();
            }
            pImpl->rtAudio.closeStream();
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
