//
// Created by Fidel on 2023/10/26.
//

#ifndef DXCAM_CPP_DUPLICATOR_H
#define DXCAM_CPP_DUPLICATOR_H

#include "Device.h"
#include "Output.h"

namespace DXCam {

class Duplicator {
public:
    Duplicator(Output *output, Device *device);
    ~Duplicator();

    bool update_frame();
    void release_frame();

    ID3D11Texture2D *texture = nullptr;
    IDXGIOutputDuplication *duplicator = nullptr;
    bool updated = false;
    Output *output = nullptr;
    Device *device = nullptr;
};

}  // namespace DXCam

#endif  //DXCAM_CPP_DUPLICATOR_H
