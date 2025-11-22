# lib-guitar-io

This file provides guidance when working with the **lib-guitar-io** library.

## Overview

**lib-guitar-io** is a cross-platform audio I/O abstraction library that wraps RtAudio to provide clean, RAII-based interfaces for real-time audio device management. It's designed for low-latency guitar audio applications.

**Key Features:**

- Cross-platform audio I/O (ASIO/WASAPI on Windows, CoreAudio on macOS, ALSA on Linux)
- RAII wrapper around RtAudio with PIMPL pattern
- Device enumeration and management
- Type-safe callback interface
- Lock-free, real-time safe audio processing

## Repository Structure

```
lib-guitar-io/
├── include/
│   ├── AudioDevice.h           # Main audio device interface
│   └── AudioDeviceManager.h    # Device enumeration
├── src/
│   ├── AudioDevice.cpp         # Implementation
│   └── AudioDeviceManager.cpp  # Device management
├── external/
│   └── rtaudio/                # RtAudio submodule
├── CMakeLists.txt
└── CLAUDE.md
```

## Coding Standards

**Naming Conventions:**

- **PascalCase** for classes, structs, and functions
  - Examples: `AudioDevice`, `OpenDefault()`, `GetLastError()`
- **camelCase** for ALL variables (including class member variables)
  - **NO trailing underscores** on member variables
  - Examples: `pImpl`, `callback`, `userData`, `lastError`

**Code Formatting:**

- **Line limit:** 120 characters
- **Indentation:** 4 spaces (no tabs)
- **Brace style:** Allman (opening braces on new lines)
- **Pointer alignment:** Right-aligned (`Type *ptr`)

**Formatting Tools:**

- `.clang-format` - Enforces code style
- `.clang-tidy` - Static analysis
- `.pre-commit-config.yaml` - Automated checks

## Architecture

### PIMPL Pattern

The library uses the Pointer to Implementation (PIMPL) idiom to hide RtAudio implementation details:

```cpp
class AudioDevice
{
public:
    AudioDevice();
    ~AudioDevice();
    // ... public interface ...

private:
    class Impl;  // Forward declaration
    std::unique_ptr<Impl> pImpl;  // Hides RtAudio details
};
```

**Benefits:**

- Stable ABI (RtAudio changes don't affect public interface)
- Faster compilation (RtAudio headers not in public headers)
- Clean separation of concerns

### Real-Time Safety

**CRITICAL:** Audio callbacks must be real-time safe:

**✅ SAFE in audio callback:**

- Reading/writing pre-allocated buffers
- Simple arithmetic
- Lock-free atomics

**❌ UNSAFE in audio callback:**

- Memory allocation (`new`, `malloc`, `std::vector::push_back`)
- Locks (`std::mutex`, `std::lock_guard`)
- File or network I/O
- Logging

### Callback Adapter Pattern

The library converts RtAudio's C-style callback to a modern C++ `std::function`:

```cpp
using AudioCallback = std::function<int(
    const float *inputBuffer,
    float *outputBuffer,
    size_t frameCount,
    void *userData
)>;
```

Internally, a static adapter bridges RtAudio callbacks to `std::function`.

## Usage Example

```cpp
#include <AudioDevice.h>
#include <AudioDeviceManager.h>

// Configure stream
GuitarIO::AudioStreamConfig config{
    .sampleRate = 48000,
    .bufferSize = 256,
    .inputChannels = 1,
    .outputChannels = 0
};

// Audio callback
auto callback = [](const float *input, float *output, size_t frames, void *userData) {
    // Process audio (real-time safe code only!)
    return 0;  // Continue stream
};

// Open and start device
GuitarIO::AudioDevice device;
if (!device.OpenDefault(config, callback, nullptr))
{
    std::cerr << "Failed: " << device.GetLastError() << std::endl;
    return 1;
}

if (!device.Start())
{
    std::cerr << "Failed to start: " << device.GetLastError() << std::endl;
    return 1;
}

// ... audio runs in background thread ...

device.Stop();
device.Close();
```

## Device Management

### Enumerating Devices

```cpp
#include <AudioDeviceManager.h>

auto &manager = GuitarIO::AudioDeviceManager::Get();
auto devices = manager.EnumerateInputDevices();

for (const auto &device : devices)
{
    std::cout << "Device: " << device.name << std::endl;
    std::cout << "  ID: " << device.id << std::endl;
    std::cout << "  Max Input Channels: " << device.maxInputChannels << std::endl;
}
```

### Platform-Specific APIs

The library automatically selects the best audio API:

| Platform | API | Latency | Notes |
|----------|-----|---------|-------|
| Windows | ASIO | 5-10ms | Best performance (requires ASIO drivers) |
| Windows | WASAPI | 10-20ms | Fallback (built into Windows) |
| macOS | CoreAudio | 2-5ms | Native, low latency |
| Linux | ALSA | 1-5ms | Best with RT kernel |

## Common Tasks

### Error Handling

```cpp
GuitarIO::AudioDevice device;

if (!device.OpenDefault(config, callback))
{
    // Check error message
    std::string error = device.GetLastError();
    LOG_ERROR("Audio device error: {}", error);
}
```

### Checking Device State

```cpp
if (device.IsOpen())
{
    // Device is open
}

if (device.IsRunning())
{
    // Stream is running
}
```

### Clean Shutdown

```cpp
// RAII handles cleanup automatically, but manual control is available:
if (device.IsRunning())
{
    device.Stop();
}

if (device.IsOpen())
{
    device.Close();
}
// Destructor also handles cleanup
```

## Development Guidelines

### Adding New Features

1. Keep the public API minimal and stable
2. Hide platform-specific details in PIMPL implementation
3. Maintain real-time safety guarantees
4. Update this documentation

### Testing

```cpp
// Test device enumeration
TEST(AudioDeviceManagerTest, EnumeratesDevices)
{
    auto &manager = GuitarIO::AudioDeviceManager::Get();
    auto devices = manager.EnumerateInputDevices();
    EXPECT_GT(devices.size(), 0);  // At least one device
}

// Test device open/close
TEST(AudioDeviceTest, OpenClose)
{
    GuitarIO::AudioDevice device;
    GuitarIO::AudioStreamConfig config{.sampleRate = 48000, .bufferSize = 256};

    auto callback = [](const float *, float *, size_t, void *) { return 0; };

    EXPECT_TRUE(device.OpenDefault(config, callback));
    EXPECT_TRUE(device.IsOpen());

    device.Close();
    EXPECT_FALSE(device.IsOpen());
}
```

## Troubleshooting

**Problem:** No audio input detected

- Check device permissions (macOS/Linux require microphone access)
- Verify correct device selected
- Check sample rate compatibility

**Problem:** Audio dropouts/crackling

- Increase buffer size (128 → 256 → 512)
- Check CPU usage
- Verify real-time thread priority (OS-specific)

**Problem:** Device enumeration fails

- Ensure RtAudio compiled correctly
- Check platform-specific audio drivers installed
- Verify audio service running (Windows Audio service, PulseAudio/ALSA on Linux)

## Build System

This library is designed to be used as a git submodule:

```cmake
# Parent project CMakeLists.txt
add_subdirectory(external/lib-guitar-io)

target_link_libraries(your-app PRIVATE guitar-io)
```

The library automatically links RtAudio and handles platform-specific dependencies.

## License

MIT License - See LICENSE file for details.

Compatible with commercial projects. RtAudio is also MIT-licensed.
