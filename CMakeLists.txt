cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP
        VERSION 0.0.1
        LANGUAGES CXX
)

# Configuration options
set(BUILD_BENCHMARKS ON CACHE BOOL "Build benchmarks")
set(BUILD_EXAMPLES ON CACHE BOOL "Build examples")
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)

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
        PUBLIC include ${PROJECT_BINARY_DIR}/exports ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(DXCam
        PUBLIC d3d11 dxgi ${OpenCV_LIBS}
)
generate_export_header(DXCam
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
)

# Install
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

message(STATUS "Configuring DXCam_CPP - done")

if (BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
