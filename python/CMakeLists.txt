message(STATUS "Configuring DXCam-CPP-Python")

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(dxcam_cpp MODULE
        src/util/numpy.cpp
        src/util/string_cvt.cpp
        src/py_bindings.cpp
        src/py_dxcam.cpp
        src/py_DXCamera.cpp
        src/py_FrameBuffer.cpp
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

message(STATUS "Configuring DXCam-CPP-Python - done")
