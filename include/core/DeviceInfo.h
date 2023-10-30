//
// Created by Fidel on 2023/10/29.
//

#ifndef DXCAM_CPP_DEVICEINFO_H
#define DXCAM_CPP_DEVICEINFO_H

#include <dxgi.h>

#include <string>

namespace DXCam {

struct DeviceInfo {
    std::wstring description;
    SIZE_T vram_size;
    UINT vendor_id;
};

}  // namespace DXCam

#endif  //DXCAM_CPP_DEVICEINFO_H
