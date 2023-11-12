
#include <chrono>
#include <cstdio>
#include <string>

#include "dxcam.h"

// params
const std::string TITLE = "[Benchmark] real_grab_cost";
const auto REGION = DXCam::Region{0, 0, 1920, 1080};
const int TOTAL_FRAMES = 100;

double bench(const std::shared_ptr<DXCam::DXCamera>& camera) {
    double total_grab_cost = 0;
    cv::Mat prev_frame;

    for (int i = 0; i < TOTAL_FRAMES;) {
        const auto begin_grab = std::chrono::steady_clock::now();
        auto frame = camera->grab();
        const auto end_grab = std::chrono::steady_clock::now();

        if (prev_frame.empty()) {
            frame.copyTo(prev_frame);
            total_grab_cost += std::chrono::duration<double>(end_grab - begin_grab).count();
        } else {
            bool is_identical = (sum(frame != prev_frame) == cv::Scalar(0, 0, 0, 0));
            if (!is_identical) {
                total_grab_cost += std::chrono::duration<double>(end_grab - begin_grab).count();
                frame.copyTo(prev_frame);
                i++;
            }
        }
    }

    return total_grab_cost;
}

int main() {
    // init
    auto camera = DXCam::create(REGION);

    // benchmark
    double total_grab_cost = std::numeric_limits<double>::max();
    total_grab_cost = std::min(total_grab_cost, bench(camera));


    // result
    const auto average_cost = total_grab_cost / TOTAL_FRAMES * 1000;
    printf("%s: %d loops, average grab cost %.3lf ms\n", TITLE.c_str(), TOTAL_FRAMES, average_cost);

    return 0;
}