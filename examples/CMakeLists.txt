message(STATUS "Configuring examples")

find_package(OpenCV REQUIRED core highgui)

add_executable(example_screenshot screenshot.cpp)
target_include_directories(example_screenshot PRIVATE ../include)
target_link_libraries(example_screenshot DXCam dxgi d3d11 ${OpenCV_LIBS})
add_dependencies(example_screenshot DXCam)

add_executable(example_video_mode video_mode.cpp)
target_include_directories(example_video_mode PRIVATE ../include)
target_link_libraries(example_video_mode DXCam dxgi d3d11 ${OpenCV_LIBS})
add_dependencies(example_video_mode DXCam)

message(STATUS "Configuring examples - done")