cmake_minimum_required(VERSION 3.20)

project(lib-guitar-io VERSION 0.0.1 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection macros
if(WIN32)
    add_compile_definitions(PLATFORM_WINDOWS)
elseif(APPLE)
    add_compile_definitions(PLATFORM_MACOS)
elseif(UNIX)
    add_compile_definitions(PLATFORM_LINUX)
endif()

# FetchContent for rtaudio
include(FetchContent)

FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG        ab7daaae763c8e0dcea611a9e190fa46d95b2639
)
# Dont build tests
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "Don't build RtAudio tests" FORCE)
FetchContent_MakeAvailable(rtaudio)

# Create guitar-io library (static)
add_library(guitar-io STATIC
    src/RtAudioDevice.cpp
    src/AudioDeviceManager.cpp
    src/SineWaveGenerator.cpp
    src/PolyphonicGenerator.cpp
    src/AudioMixer.cpp
)

target_include_directories(guitar-io PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link RtAudio
target_link_libraries(guitar-io PUBLIC rtaudio)

# Platform-specific audio API configuration
if(WIN32)
    # ASIO support on Windows (preferred for low latency)
    # WASAPI as fallback
    message(STATUS "lib-guitar-io: Windows audio (ASIO/WASAPI)")
elseif(APPLE)
    # CoreAudio on macOS
    target_link_libraries(guitar-io PUBLIC
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework CoreFoundation"
    )
    message(STATUS "lib-guitar-io: macOS CoreAudio")
elseif(UNIX)
    # ALSA on Linux
    find_package(ALSA REQUIRED)
    target_link_libraries(guitar-io PUBLIC ${ALSA_LIBRARIES})
    message(STATUS "lib-guitar-io: Linux ALSA")
endif()

# Compiler warnings
if(MSVC)
    target_compile_options(guitar-io PRIVATE /W4 /WX)
else()
    target_compile_options(guitar-io PRIVATE
        -Wall -Wextra -Wpedantic -Werror
        -Wno-unused-parameter
    )
endif()
