cmake_minimum_required(VERSION 3.15)
project(DXCam_CPP
        VERSION 0.2.1
        LANGUAGES CXX
)

# Set default install directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
endif ()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if (EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif ()

# Configuration options
set(BUILD_BENCHMARKS OFF CACHE BOOL "Build benchmarks")
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples")
set(BUILD_PYTHON OFF CACHE BOOL "Build Python bindings")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(INSTALL_PYTHON_ONLY OFF CACHE BOOL "Install Python module ONLY")

set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR})

# Check dependencies
# Require MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "DXCam_CPP requires MSVC")
endif ()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "Configuring DXCam_CPP")

# Set compiler options
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Od /fsanitize=address /Zi)
else ()
    add_compile_options(/O2 /GL)
    add_link_options(/LTCG)
endif ()

find_package(OpenCV REQUIRED core imgcodecs)

# DXCam
add_library(DXCam
        ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
        include/dxcam/core/Device.h
        include/dxcam/core/DeviceInfo.h
        include/dxcam/core/Duplicator.h
        include/dxcam/core/Output.h
        include/dxcam/core/OutputInfo.h
        include/dxcam/core/Region.h
        include/dxcam/core/StageSurface.h
        include/dxcam/dxcam.h
        include/dxcam/DXCamera.h
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
        include/dxcam
        ${OpenCV_INCLUDE_DIRS}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/exports>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(DXCam
        PUBLIC d3d11 dxgi ${OpenCV_LIBS}
)
target_compile_features(DXCam
        PUBLIC cxx_std_20
)
set_target_properties(DXCam
        PROPERTIES CXX_EXTENSIONS OFF
        DEBUG_POSTFIX d
)
include(GenerateExportHeader)
generate_export_header(DXCam
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
)

# Install
if (NOT INSTALL_PYTHON_ONLY)
    install(TARGETS DXCam
            EXPORT DXCamTargets
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(DIRECTORY include DESTINATION .)
    install(FILES
            ${PROJECT_BINARY_DIR}/exports/dxcam_export.h
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            DXCamConfigVersion.cmake
            VERSION ${PACKAGE_VERSION}
            COMPATIBILITY AnyNewerVersion
    )
    install(EXPORT DXCamTargets
            FILE DXCamTargets.cmake
            NAMESPACE DXCam::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxcam
    )
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DXCamConfig.cmake.in
            "${CMAKE_CURRENT_BINARY_DIR}/DXCamConfig.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxcam
            PATH_VARS INCLUDE_INSTALL_DIR
    )
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/DXCamConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/DXCamConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dxcam
    )

    # Export
    export(TARGETS DXCam
            NAMESPACE DXCam::
            FILE DXCamTargets.cmake
    )
    export(PACKAGE DXCam)
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
