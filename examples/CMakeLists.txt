message(STATUS "Configuring examples")

add_executable(example_screenshot screenshot.cpp)
target_include_directories(example_screenshot PRIVATE ../include)
target_link_libraries(example_screenshot DXCam dxgi d3d11 ${OpenCV_LIBS})
add_dependencies(example_screenshot DXCam)

message(STATUS "Configuring examples - done")