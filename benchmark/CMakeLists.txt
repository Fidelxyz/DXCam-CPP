cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP_benchmark)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Building benchmark")

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

include_directories(
        ../include
)
link_directories(${CMAKE_BINARY_DIR}/lib)
add_executable(DXCam_CPP_benchmark
        src/main.cpp
)
target_link_libraries(DXCam_CPP_benchmark
        DXCam_static
        ${OpenCV_LIBS}
)

add_dependencies(DXCam_CPP_benchmark DXCam_static)

message(STATUS "Building benchmark - done")