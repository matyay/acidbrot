cmake_minimum_required(VERSION 3.2)
project(AcidBrot VERSION 0.1.0)

# Options
option(TRACE "Enable spdlog trace level" OFF)
option(PROFILER "Enable profiler" OFF)

# Set the default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Initialize GIT submodules
execute_process(COMMAND git submodule update --init --recursive)

# =============================================================================

# Detect system architecture
execute_process(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCH)
message(STATUS "Architecture: ${ARCH}")

# Print platform
if(${ARCH} STREQUAL "x86_64")
    set(PLATFORM "PC")
elseif(${ARCH} STREQUAL "armv7l")
    set(PLATFORM "RaspberryPi")
else()
    message(FATAL_ERROR "This architecture is not supported!")
endif()

message(STATUS "Build type  : ${CMAKE_BUILD_TYPE}")
message(STATUS "Profiler    : ${PROFILER}")
message(STATUS "Trace       : ${TRACE}")

# =============================================================================

# Setup common compilation flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Wno-unused-function")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math -O3")

# Platform specific flags
if(${ARCH} STREQUAL "armv7l")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard")
endif()

# Add symbols if requested
if(DEBUG_SYMBOLS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   
endif()

# Common include and link directories
include_directories(src)
include_directories(3rd_party)
include_directories(3rd_party/spdlog/include)
include_directories("/usr/include/freetype2") # A hack for libfreetype

# Common defines
add_definitions(-DARCH="${ARCH}")

# Enable trace
if(TRACE)
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
endif()

# Enable profiling
if(PROFILER)
    add_definitions(-DPROFILER_ENABLE=1)
endif()

# =============================================================================

# Include 3rd party library targets
add_subdirectory(3rd_party)

# 3rd party libs
set(THIRD_PARTY_LIBS
    spdlog
    stb
    png
)

# Common libs
set(COMMON_LIBS
    pthread
    freetype
)

# OpenGL libs
set(GL_LIBS
    GL
    GLU
    glfw
    X11
    dl
)

# =============================================================================

file (GLOB_RECURSE SRCS src/*.c src/*.cc)

add_executable(acidbrot ${SRCS})

target_link_libraries(acidbrot PRIVATE
    ${COMMON_LIBS}
    ${GL_LIBS}
    ${THIRD_PARTY_LIBS}
)
