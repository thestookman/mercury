
cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Mercury VERSION 1.0.0)

# Find all C++ source files inside src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Build the Mercury mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Find the Geode SDK
if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR
        "Unable to find Geode SDK! "
        "Please define GEODE_SDK to point to your Geode SDK."
    )
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

# Load the Geode SDK build system
add_subdirectory(
    $ENV{GEODE_SDK}
    ${CMAKE_CURRENT_BINARY_DIR}/geode
)

# Configure Mercury as a Geode mod
setup_geode_mod(${PROJECT_NAME})
