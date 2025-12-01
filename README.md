# lib-guitar-io

[![Build Status](https://github.com/Konstantysz/kappa-core/actions/workflows/build.yml/badge.svg)](https://github.com/Konstantysz/kappa-core/actions/workflows/build.yml)
[![Tests](https://github.com/Konstantysz/kappa-core/actions/workflows/tests.yml/badge.svg)](https://github.com/Konstantysz/kappa-core/actions/workflows/tests.yml)
[![Static Analysis](https://github.com/Konstantysz/kappa-core/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/Konstantysz/kappa-core/actions/workflows/static-analysis.yml)

Cross-platform audio I/O library for guitar applications.

## Overview

`lib-guitar-io` provides a clean C++20 interface for low-latency audio input/output, abstracting platform-specific audio APIs:

- **Windows**: ASIO (preferred) / WASAPI (fallback)
- **macOS**: CoreAudio
- **Linux**: ALSA

Built on [RtAudio](https://github.com/thestk/rtaudio) with an interface-based design for testability and flexibility.

## Features

- ✅ Cross-platform audio I/O (Windows/macOS/Linux)
- ✅ Low-latency audio input (<10ms on ASIO/CoreAudio)
- ✅ Device enumeration and configuration
- ✅ Clean C++20 API with RAII resource management
- ✅ Type-safe callbacks with user data
- ✅ MIT License (commercially compatible)

## Usage

```cpp
#include <RtAudioDevice.h>
#include <AudioDeviceManager.h>

using namespace GuitarIO;

// Enumerate devices
auto& manager = AudioDeviceManager::Get();
auto devices = manager.EnumerateInputDevices();

// Audio callback (uses std::span for type safety)
int audioCallback(std::span<const float> input, std::span<float> output, void* userData)
{
    // Process audio here (real-time constraints apply!)
    for (size_t i = 0; i < input.size(); ++i)
    {
        // Example: passthrough
        if (!output.empty())
        {
            output[i] = input[i];
        }
    }
    return 0; // Continue
}

// Open and start audio stream
RtAudioDevice device;
AudioStreamConfig config{
    .sampleRate = 48000,
    .bufferSize = 512,
    .inputChannels = 1,
    .outputChannels = 0
};

if (device.OpenDefault(config, audioCallback))
{
    device.Start();
    // ... process audio ...
    device.Stop();
}
```

## Building

This library is designed to be used as a git submodule:

```bash
# Add as submodule
git submodule add <repo-url> external/lib-guitar-io
git submodule update --init --recursive

# CMake integration
add_subdirectory(external/lib-guitar-io)
target_link_libraries(your-app PRIVATE guitar-io)
```

## Dependencies

- **RtAudio** (git submodule): Cross-platform audio I/O
- **CMake** 3.20+
- **C++20** compiler

### Platform-specific dependencies

**Linux:**

```bash
# ALSA development libraries
sudo apt install libasound2-dev  # Ubuntu/Debian
sudo dnf install alsa-lib-devel  # Fedora
sudo pacman -S alsa-lib          # Arch
```

**macOS/Windows:** No additional dependencies (CoreAudio/WASAPI built-in)

## Architecture

```text
lib-guitar-io/
├── include/
│   ├── AudioDevice.h         # Abstract audio device interface
│   ├── RtAudioDevice.h       # RtAudio implementation
│   └── AudioDeviceManager.h  # Device enumeration
├── src/
│   ├── RtAudioDevice.cpp     # RtAudio implementation
│   └── AudioDeviceManager.cpp
├── external/
│   └── rtaudio/              # RtAudio submodule
└── CMakeLists.txt
```

The library uses an **interface-based design**:

- `AudioDevice` - Abstract interface (pure virtual methods)
- `RtAudioDevice` - Concrete implementation using RtAudio
- `MockAudioDevice` - Test implementation (in consuming projects)

This design enables dependency injection and unit testing without requiring real audio hardware.

## Real-Time Safety

**IMPORTANT:** The audio callback runs in a real-time thread with strict constraints:

❌ **DO NOT** in callback:

- Allocate memory (`new`, `malloc`, `std::vector::push_back`)
- Acquire locks (`std::mutex`, etc.)
- Perform I/O (file, network, logging)
- Call blocking functions

✅ **DO** in callback:

- Use pre-allocated buffers
- Use lock-free data structures
- Perform signal processing
- Read/write atomics

See [CLAUDE.md](../../CLAUDE.md) for detailed real-time audio guidelines.

## License

MIT License - See [LICENSE](LICENSE) for details.

Uses RtAudio (MIT-like license).
