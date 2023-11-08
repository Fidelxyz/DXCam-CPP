cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP)

set(CMAKE_CXX_STANDARD 20)

# Require MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "DXCam_CPP requires MSVC")
endif ()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "Configuring DXCam_CPP")

# Set target directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set compiler options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /fsanitize=address)
else ()
    add_compile_options(/O2)
endif ()

find_package(OpenCV REQUIRED)

include_directories(include src
        ${OpenCV_INCLUDE_DIRS}
)

add_library(DXCam STATIC
        include/core/Device.h
        include/core/DeviceInfo.h
        include/core/Duplicator.h
        include/core/Output.h
        include/core/OutputInfo.h
        include/core/Processor.h
        include/core/Region.h
        include/core/StageSurface.h
        include/dxcam.h
        include/DXCamera.h
        src/DXFactory.h
        src/core/Device.cpp
        src/core/Duplicator.cpp
        src/core/Output.cpp
        src/core/OutputMetadata.cpp
        src/core/OutputMetadata.h
        src/core/Processor.cpp
        src/core/Region.cpp
        src/core/StageSurface.cpp
        src/util/io.cpp
        src/util/io.h
        src/dxcam.cpp
        src/DXCamera.cpp
        src/DXFactory.cpp
        include/core/DeviceInfo.h
        include/core/OutputInfo.h
        examples/src/screenshot.cpp
        src/util/HighResTimer.cpp
        src/util/HighResTimer.h)

target_link_libraries(DXCam
        d3d11
        dxgi
        ${OpenCV_LIBS})

message(STATUS "Configuring DXCam_CPP - done")

add_subdirectory(benchmark)
add_subdirectory(examples)