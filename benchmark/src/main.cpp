#include <chrono>
#include <cstdio>
#include <string>

#include "dxcam.h"

int main() {
    // params
    std::string title = "[DXcam] FPS benchmark";
    const auto region = DXCam::Region{0, 0, 1920, 1080};
    const int total_frames = 1000;

    // init
    auto camera = DXCam::create(region, 0, 0);

    // benchmark
    const auto begin_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < total_frames;) {
        auto frame = camera->grab();
        if (!frame.empty()) { i++; }  // new frame
    }
    const auto end_time = std::chrono::high_resolution_clock::now();

    // result
    const auto fps =
            total_frames /
            std::chrono::duration<double>(end_time - begin_time).count();
    printf("%s: %f FPS\n", title.c_str(), fps);

    return 0;
}