#ifndef DXCAM_CPP_OUTPUTINFO_H
#define DXCAM_CPP_OUTPUTINFO_H

#include <dxgi.h>

#include <string>

namespace DXCam {

struct OutputInfo {
    [[maybe_unused]] std::wstring device_name;
    [[maybe_unused]] LONG width;
    [[maybe_unused]] LONG height;
    [[maybe_unused]] int rotation_angle;
    [[maybe_unused]] bool is_primary;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_OUTPUTINFO_H
