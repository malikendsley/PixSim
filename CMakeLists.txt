cmake_minimum_required(VERSION 3.31)
project(PixSim)

# Use clang as the default compiler if not explicitly set
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# Find SDL2 and SDL2_mixer
find_package(SDL2 REQUIRED PATHS "C:/cpplibs/SDL2")
find_package(SDL2_mixer REQUIRED PATHS "C:/cpplibs/SDL2_mixer-2.8.0")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Default to Debug build type if none is set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type (Debug, Release, etc.)" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(PixSim ${SOURCES})

target_link_libraries(PixSim PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
)

set_target_properties(PixSim PROPERTIES
    CXX_STANDARD 17
)

# Configure build flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(PixSim PRIVATE
        -g                       # Include debug symbols
        -Og                      # Optimize for debugging
        -Wall -Wextra -Wpedantic # Enable warnings
    )
    target_link_options(PixSim PRIVATE
        -g                       # Retain debug information in the linker stage
    )
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(PixSim PRIVATE
        -O3                      # Aggressive optimizations
        -DNDEBUG                 # Disable assertions
        -march=native            # Optimize for the current CPU architecture
        -Wall -Wextra -Wpedantic # Enable warnings
    )
    target_link_options(PixSim PRIVATE
        -O3                      # Aggressive optimizations in the linker stage
    )
endif()

# Copy DLLs and assets to the bin directory
add_custom_command(TARGET PixSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SDL2.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SDL2_mixer.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
)
