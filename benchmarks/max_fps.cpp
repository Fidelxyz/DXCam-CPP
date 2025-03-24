#include <chrono>
#include <cstdio>
#include <string>

#include "dxcam/dxcam.h"

// params
const std::string TITLE = "[Benchmark] max_fps";
constexpr auto REGION = DXCam::Region{0, 0, 1920, 1080};
constexpr int TOTAL_FRAMES = 100;
constexpr int REPEATS = 5;

double bench(const std::shared_ptr<DXCam::DXCamera>& camera) {
    const auto begin_time = std::chrono::steady_clock::now();
    for (int i = 0; i < TOTAL_FRAMES;) {
        auto frame = camera->grab();
        if (!frame.empty()) {
            volatile auto data = std::move(frame);  // avoid optimization
            i++;
        }
    }
    const auto end_time = std::chrono::steady_clock::now();

    return std::chrono::duration<double>(end_time - begin_time).count();
}

int main() {
    // init
    const auto camera = DXCam::create(REGION);

    // benchmarks
    double duration = std::numeric_limits<double>::max();
    for (int i = 0; i < REPEATS; i++) {
        printf("Bench [%d / %d]\n", i + 1, REPEATS);
        duration = std::min(duration, bench(camera));
    }

    // result
    const auto fps = TOTAL_FRAMES / duration;
    printf("%s: %d loops, best of %d: %lf FPS\n", TITLE.c_str(), TOTAL_FRAMES,
           REPEATS, fps);

    return 0;
}