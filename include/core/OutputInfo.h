//
// Created by Fidel on 2023/10/29.
//

#ifndef DXCAM_CPP_OUTPUTINFO_H
#define DXCAM_CPP_OUTPUTINFO_H

#include <dxgi.h>

#include <string>

struct OutputInfo {
    std::wstring device_name;
    LONG width;
    LONG height;
    int rotation_angle;
};

#endif  //DXCAM_CPP_OUTPUTINFO_H
