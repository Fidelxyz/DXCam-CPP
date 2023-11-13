cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP
        VERSION 0.0.1
        LANGUAGES CXX
)

# Configuration options
set(BUILD_BENCHMARKS ON CACHE BOOL "Build benchmarks")
set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(BUILD_PYTHON ON CACHE BOOL "Build Python bindings")

set(CMAKE_CXX_STANDARD 20)

# Require MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "DXCam_CPP requires MSVC")
endif ()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "Configuring DXCam_CPP")

# Set target directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Set compiler options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /fsanitize=address)
else ()
    add_compile_options(/O2 /GL)
    add_link_options(/LTCG)
endif ()

find_package(OpenCV REQUIRED)

add_library(DXCam SHARED
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
        src/core/Device.cpp
        src/core/Duplicator.cpp
        src/core/Output.cpp
        src/core/OutputMetadata.cpp
        src/core/OutputMetadata.h
        src/core/Processor.cpp
        src/core/Region.cpp
        src/core/StageSurface.cpp
        src/util/HighResTimer.cpp
        src/util/io.cpp
        src/util/io.h
        src/dxcam.cpp
        src/DXCamera.cpp
        src/DXFactory.cpp
        src/DXFactory.h
)
target_include_directories(DXCam
        PRIVATE src
        PUBLIC include ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(DXCam
        PUBLIC d3d11 dxgi ${OpenCV_LIBS})

# Collect DLLs
add_custom_command(TARGET DXCam POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:DXCam> $<TARGET_RUNTIME_DLLS:DXCam>
        COMMAND_EXPAND_LISTS
)

message(STATUS "Configuring DXCam_CPP - done")

if (BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (BUILD_PYTHON)
    add_subdirectory(python)
endif ()
