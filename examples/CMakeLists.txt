cmake_minimum_required(VERSION 3.26)
project(DXCam_CPP_examples)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "Building examples")

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

add_executable(screenshot src/screenshot.cpp)
target_link_libraries(screenshot DXCam ${OpenCV_LIBS})
add_dependencies(screenshot DXCam)

message(STATUS "Building examples - done")