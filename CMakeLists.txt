cmake_minimum_required(VERSION 3.15)
project(PixSim)

find_package(SDL2 REQUIRED PATHS "C:/cpplibs/SDL2")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/release)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(PixSim ${SOURCES})

target_link_libraries(PixSim PRIVATE ${SDL2_LIBRARIES})

set_target_properties(PixSim PROPERTIES CXX_STANDARD 17)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PixSim)
endif()

add_custom_command(TARGET PixSim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/SDL2.dll" $<TARGET_FILE_DIR:PixSim>
)