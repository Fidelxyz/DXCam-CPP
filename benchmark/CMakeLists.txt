cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP_benchmark)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Configuring benchmark")

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

include_directories(../include)
link_directories(${CMAKE_BINARY_DIR}/lib)

# Benchmark: max_fps
add_executable(benchmark_max_fps src/max_fps.cpp)
target_link_libraries(benchmark_max_fps
        DXCam
        ${OpenCV_LIBS}
)
add_dependencies(benchmark_max_fps DXCam)

# Benchmark: target_fps
add_executable(benchmark_target_fps src/target_fps.cpp)
target_link_libraries(benchmark_target_fps
        DXCam
        ${OpenCV_LIBS}
)
add_dependencies(benchmark_target_fps DXCam)

message(STATUS "Configuring benchmark - done")