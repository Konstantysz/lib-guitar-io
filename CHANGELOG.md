# Changelog

All notable changes to lib-guitar-io will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.0.1] - 2025-12-06

### Added

- Cross-platform audio I/O via RtAudio wrapper
- RAII device management with automatic resource cleanup
- Device enumeration and hot-plug detection support
- std::span-based type-safe audio buffer interfaces
- Platform abstraction for ASIO (Windows), CoreAudio (macOS), ALSA (Linux)
- Lock-free ring buffer for audio thread communication
- Real-time safe audio callback interface

### Technical

- C++20 codebase with RAII patterns
- Thread-safe device management
- Zero-allocation audio callback path
- MIT-like permissive licensing (RtAudio)
