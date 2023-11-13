message(STATUS "Configuring benchmark")

# Benchmark: max_fps
add_executable(benchmark_max_fps max_fps.cpp)
target_link_libraries(benchmark_max_fps DXCam)
add_dependencies(benchmark_max_fps DXCam)

# Benchmark: target_fps
add_executable(benchmark_target_fps target_fps.cpp)
target_link_libraries(benchmark_target_fps DXCam)
add_dependencies(benchmark_target_fps DXCam)

message(STATUS "Configuring benchmark - done")