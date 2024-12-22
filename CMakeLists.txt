cmake_minimum_required(VERSION 3.0)
project(PixSim)



find_package(SDL2 REQUIRED PATHS "C:/cpplibs/SDL2")
find_package(SDL2_mixer REQUIRED PATHS "C:/cpplibs/SDL2_mixer-2.8.0")
message(STATUS "SDL2 Dir: ${SDL2_DIR}")
message(STATUS "SDL2_mixer Dir: ${SDL2_mixer_DIR}")
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

add_executable(PixSim ${SOURCES})

target_link_libraries(PixSim PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
)

set_target_properties(PixSim PROPERTIES CXX_STANDARD 17)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PixSim)
endif()

add_custom_command(TARGET PixSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SDL2.dll" $<TARGET_FILE_DIR:PixSim>
)

add_custom_command(TARGET PixSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SDL2_mixer.dll" $<TARGET_FILE_DIR:PixSim>
)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build architecture: ${CMAKE_SIZEOF_VOID_P} bytes")