#include "dxcam/dxcam.h"
#include "opencv2/opencv.hpp"

int main() {
    const auto devices_info = DXCam::get_devices_info();
    const auto outputs_info = DXCam::get_outputs_info();

    for (size_t i = 0; i < devices_info.size(); i++) {
        const auto &[description, vram_size, vendor_id] = devices_info[i];
        printf("Device [%llu]: %ls VRam:%llu VendorId:%u\n", i,
               description.c_str(), vram_size, vendor_id);
        for (size_t j = 0; j < outputs_info[i].size(); j++) {
            const auto &[device_name, width, height, rotation_angle,
                         is_primary] = outputs_info[i][j];
            printf("\tOutput [%llu]: %ls Res:%ldx%ld Rot:%d IsPrimary:%d\n", j,
                   device_name.c_str(), width, height, rotation_angle,
                   is_primary);
        }
    }

    const auto camera = DXCam::create();

    while (true) {
        auto frame = camera->grab();
        if (frame.empty()) {
            std::cerr << "Failed to grab frame." << std::endl;
            continue;
        }

        cv::imshow("frame", frame);
        if (cv::waitKey(0) == 27) {  // ESC
            break;
        }
    }

    return 0;
}