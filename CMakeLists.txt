cmake_minimum_required(VERSION 3.27)
project(DXCam_CPP
        VERSION 0.1.2
        LANGUAGES CXX
)

# Configuration options
set(BUILD_BENCHMARKS OFF CACHE BOOL "Build benchmarks")
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(BUILD_PYTHON OFF CACHE BOOL "Build Python bindings")
set(INSTALL_PYTHON_ONLY OFF CACHE BOOL "Install Python module ONLY")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)

# Check dependencies
# Require MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "DXCam_CPP requires MSVC")
endif ()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "Configuring DXCam_CPP")

set(CMAKE_CXX_STANDARD 20)

# Set target directories
if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()
if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif ()
if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif ()

# Set compiler options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /fsanitize=address)
else ()
    add_compile_options(/O2 /GL)
    add_link_options(/LTCG)
endif ()

include(GenerateExportHeader)

find_package(OpenCV REQUIRED)

# DXCam
add_library(DXCam SHARED
        ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
        include/core/Device.h
        include/core/DeviceInfo.h
        include/core/Duplicator.h
        include/core/Output.h
        include/core/OutputInfo.h
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
        src/core/Processor.h
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
        PRIVATE
        src
        PUBLIC
        include
        ${PROJECT_BINARY_DIR}/exports
        ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(DXCam
        PUBLIC d3d11 dxgi ${OpenCV_LIBS}
)
generate_export_header(DXCam
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
)

# Install
if (NOT INSTALL_PYTHON_ONLY)
    install(TARGETS DXCam
            EXPORT DXCamTargets
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
    )
    install(DIRECTORY include
            DESTINATION .
    )
    install(FILES ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
            DESTINATION include
    )
    install(FILES $<TARGET_RUNTIME_DLLS:DXCam>
            DESTINATION bin
    )
endif ()

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