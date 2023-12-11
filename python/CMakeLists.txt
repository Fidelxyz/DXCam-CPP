message(STATUS "Configuring DXCam-CPP-Python - done")

add_subdirectory(pybind11)

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