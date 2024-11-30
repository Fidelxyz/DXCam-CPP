#include "dxcam/dxcam.h"
#include "opencv2/opencv.hpp"

int main() {
    const auto devices_info = DXCam::get_devices_info();
    const auto outputs_info = DXCam::get_outputs_info();

    for (size_t i = 0; i < devices_info.size(); i++) {
        const auto &device_info = devices_info[i];
        printf("Device [%llu]: %ls VRam:%llu VendorId:%u\n", i,
               device_info.description.c_str(), device_info.vram_size,
               device_info.vendor_id);
        for (size_t j = 0; j < outputs_info[i].size(); j++) {
            const auto &output_info = outputs_info[i][j];
            printf("\tOutput [%llu]: %ls Res:%ldx%ld Rot:%d IsPrimary:%d\n", j,
                   output_info.device_name.c_str(), output_info.width,
                   output_info.height, output_info.rotation_angle,
                   output_info.is_primary);
        }
    }

    auto camera = DXCam::create();

    while (true) {
        auto frame = camera->grab();
        cv::imshow("frame", frame);
        cv::waitKey(0);
    }

    return 0;
}