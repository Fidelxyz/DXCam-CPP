message(STATUS "Configuring DXCam-CPP-Python")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/python)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/python)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/python)

add_subdirectory(pybind11)

pybind11_add_module(dxcam_cpp MODULE
        src/util/string.cpp
        src/util/string.h
        src/py_dxcam.cpp
)
target_link_libraries(dxcam_cpp PRIVATE DXCam)
add_dependencies(dxcam_cpp DXCam)

# Collect DLLs
add_custom_command(TARGET dxcam_cpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:dxcam_cpp> $<TARGET_RUNTIME_DLLS:dxcam_cpp>
        COMMAND_EXPAND_LISTS
)
# Collect clang_rt.asan_dynamic-x86_64.dll for Debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_file(ASAN_RUNTIME "clang_rt.asan_dynamic-x86_64.dll" PATHS${CMAKE_MSVC_RUNTIME_LIBRARY})
    message(STATUS "ASAN_RUNTIME: ${ASAN_RUNTIME}")
    add_custom_command(TARGET dxcam_cpp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASAN_RUNTIME} $<TARGET_FILE_DIR:dxcam_cpp>
            COMMAND_EXPAND_LISTS
    )
endif ()

message(STATUS "Configuring DXCam-CPP-Python - done")