cmake_minimum_required(VERSION 3.26)
project(DXCam-CPP-Python
        LANGUAGES CXX
)

# Check dependencies
# Require MSVC
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "DXCam_CPP requires MSVC")
endif ()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(pybind11)
find_package(DXCam REQUIRED)
find_package(OpenCV REQUIRED)

pybind11_add_module(dxcam_cpp MODULE
        src/util/string.cpp
        src/util/string.h
        src/py_bindings.cpp
        src/py_dxcam.cpp
        src/py_dxcam.h
        src/py_DXCamera.cpp
        src/py_DXCamera.h
)
target_link_libraries(dxcam_cpp PRIVATE DXCam)
add_dependencies(dxcam_cpp DXCam)

# Collect DLLs
install(TARGETS dxcam_cpp
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION .
)
install(FILES $<TARGET_RUNTIME_DLLS:dxcam_cpp>
        DESTINATION .
)
# Collect clang_rt.asan_dynamic-x86_64.dll for Debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_file(ASAN_RUNTIME "clang_rt.asan_dynamic-x86_64.dll" PATHS ${CMAKE_MSVC_RUNTIME_LIBRARY})
    message(STATUS "ASAN_RUNTIME: ${ASAN_RUNTIME}")
    install(FILES ${ASAN_RUNTIME}
            DESTINATION .
    )
endif ()

message(STATUS "Configuring DXCam-CPP-Python - done")