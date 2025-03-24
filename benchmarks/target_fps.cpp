#include "dxcam/dxcam.h"

// params
const std::string TITLE = "target_fps";
constexpr auto REGION = DXCam::Region{0, 0, 1920, 1080};
constexpr int DURATION = 5;
constexpr int TARGET_FPS[] = {30, 60, 90, 120, 165};

std::tuple<double, double> bench(const std::shared_ptr<DXCam::DXCamera> &camera,
                                 const int target_fps) {
    const int total_frames = target_fps * DURATION;
    double square_sum = 0;

    camera->start(target_fps, true);

    const auto begin_time = std::chrono::steady_clock::now();
    auto last_frame_time = begin_time;
    for (int i = 0; i < total_frames; i++) {
        // volatile to avoid optimization
        volatile auto frame = camera->get_latest_frame();

        const auto current_time = std::chrono::steady_clock::now();
        square_sum += std::pow(
            std::chrono::duration<double>(current_time - last_frame_time)
                .count(),
            2);
        last_frame_time = current_time;
    }
    const auto end_time = std::chrono::steady_clock::now();

    camera->stop();

    const auto duration =
        std::chrono::duration<double>(end_time - begin_time).count();
    const auto fps = total_frames / duration;

    const auto std = std::sqrt(square_sum / total_frames -
                               std::pow(duration / total_frames, 2));

    return {fps, std};
}

int main() {
    std::printf("[DXCam-CPP Benchmark] %s\n\n", TITLE.c_str());

    std::puts("Parameters:");
    std::printf("  Region: (%d, %d, %d, %d)\n", REGION.left, REGION.top,
                REGION.right, REGION.bottom);
    std::printf("  Duration: %d s\n", DURATION);

    // init
    const auto camera = DXCam::create(REGION);

    // benchmark
    for (const auto &target_fps : TARGET_FPS) {
        std::printf("Bench: Target FPS = %d\n", target_fps);
        const auto [fps, std] = bench(camera, target_fps);
        std::printf("Result: Target FPS = %d: %lf FPS, std = %lf\n", target_fps,
                    fps, std);
    }

    return 0;
}