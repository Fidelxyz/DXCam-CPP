project(DXCam_CPP_examples)

message(STATUS "Configuring examples")

add_executable(screenshot src/screenshot.cpp)
target_link_libraries(screenshot DXCam ${OpenCV_LIBS})
add_dependencies(screenshot DXCam)

message(STATUS "Configuring examples - done")