message(STATUS "Configuring DXCam-CPP-Python")

find_package(Python COMPONENTS Interpreter Development)
add_subdirectory(pybind11)

pybind11_add_module(dxcam_cpp MODULE
        src/util/numpy.cpp
        src/util/numpy.h
        src/util/string_cvt.cpp
        src/util/string_cvt.h
        src/py_bindings.cpp
        src/py_dxcam.cpp
        src/py_dxcam.h
        src/py_DXCamera.cpp
        src/py_DXCamera.h
        src/py_FrameBuffer.cpp
        src/py_FrameBuffer.h
)
target_link_libraries(dxcam_cpp PRIVATE DXCam)
add_dependencies(dxcam_cpp DXCam)

if (INSTALL_PYTHON_ONLY)
    set(PYTHON_INSTALL_DIR dxcam_cpp)
else ()
    set(PYTHON_INSTALL_DIR python/dxcam_cpp)
endif ()

# Collect DLLs
install(TARGETS dxcam_cpp
        RUNTIME DESTINATION ${PYTHON_INSTALL_DIR}
        LIBRARY DESTINATION ${PYTHON_INSTALL_DIR}
        ARCHIVE DESTINATION ${PYTHON_INSTALL_DIR}
)
install(FILES $<TARGET_RUNTIME_DLLS:dxcam_cpp>
        DESTINATION ${PYTHON_INSTALL_DIR}
)
# Collect clang_rt.asan_dynamic-x86_64.dll for Debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_file(ASAN_RUNTIME "clang_rt.asan_dynamic-x86_64.dll" PATHS ${CMAKE_MSVC_RUNTIME_LIBRARY})
    message(STATUS "ASAN_RUNTIME: ${ASAN_RUNTIME}")
    install(FILES ${ASAN_RUNTIME}
            DESTINATION ${PYTHON_INSTALL_DIR}
    )
endif ()

message(STATUS "Configuring DXCam-CPP-Python - done")