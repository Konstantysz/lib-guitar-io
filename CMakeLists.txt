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

# Git submodule initialization (for RtAudio)
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Initializing RtAudio submodule...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}")
        endif()
    endif()
endif()

# Verify RtAudio submodule exists
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/rtaudio/CMakeLists.txt")
    message(FATAL_ERROR "RtAudio submodule not found. Run: git submodule update --init --recursive")
endif()

# Add RtAudio (disable building examples and tests)
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "Don't build RtAudio tests")
add_subdirectory(external/rtaudio)

# Create guitar-io library (static)
add_library(guitar-io STATIC
    src/AudioDevice.cpp
    src/AudioDeviceManager.cpp
    src/SineWaveGenerator.cpp
    src/PolyphonicGenerator.cpp
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
