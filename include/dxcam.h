//
// Created by Fidel on 2023/10/26.
//

#ifndef DXCAM_CPP_DXCAM_H
#define DXCAM_CPP_DXCAM_H

#include "DXCamera.h"

namespace DXCam {

[[maybe_unused]] std::shared_ptr<DXCamera> create(
        int device_idx = 0, int output_idx = -1, ColorFormat output_color = RGB,
        size_t max_buffer_len = 64);
[[maybe_unused]] std::shared_ptr<DXCamera> create(
        const Region &region, int device_idx = 0, int output_idx = -1,
        ColorFormat output_color = RGB, size_t max_buffer_len = 64);

[[maybe_unused]] std::vector<DeviceInfo> get_devices_info();
[[maybe_unused]] std::vector<std::vector<OutputInfo>> get_outputs_info();

}  // namespace DXCam


#endif  //DXCAM_CPP_DXCAM_H
