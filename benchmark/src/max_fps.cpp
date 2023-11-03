#include <chrono>
#include <cstdio>
#include <string>

#include "dxcam.h"

double bench(const std::shared_ptr<DXCam::DXCamera>& camera,
             const int total_frames) {
    const auto begin_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < total_frames;) {
        auto frame = camera->grab();
        if (!frame.empty()) { i++; }  // new frame
    }
    const auto end_time = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double>(end_time - begin_time).count();
}

int main() {
    // params
    std::string title = "[DXcam] FPS benchmark";
    const auto region = DXCam::Region{0, 0, 1920, 1080};
    const int total_frames = 10000;
    const int repeat = 5;

    // init
    auto camera = DXCam::create(region);

    // benchmark
    double duration = std::numeric_limits<double>::max();
    for (int i = 0; i < repeat; i++) {
        printf("Bench [%d / %d]\n", i + 1, repeat);
        duration = std::min(duration, bench(camera, total_frames));
    }

    // result
    const auto fps = total_frames / duration;
    printf("%s: %d loops, best of %d: %lf FPS\n", title.c_str(), total_frames,
           repeat, fps);

    return 0;
}