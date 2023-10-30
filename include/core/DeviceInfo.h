//
// Created by Fidel on 2023/10/29.
//

#ifndef DXCAM_CPP_DEVICEINFO_H
#define DXCAM_CPP_DEVICEINFO_H

#include <dxgi.h>

#include <string>

namespace DXCam {

struct DeviceInfo {
    [[maybe_unused]] std::wstring description;
    [[maybe_unused]] SIZE_T vram_size;
    [[maybe_unused]] UINT vendor_id;
};

}  // namespace DXCam

#endif  //DXCAM_CPP_DEVICEINFO_H
