//
// Created by Fidel on 2023/10/26.
//

#ifndef DXCAM_CPP_STAGESURFACE_H
#define DXCAM_CPP_STAGESURFACE_H

#include <d3d11.h>
#include <dxgi.h>

#include "Device.h"
#include "Output.h"

namespace DXCam {

class StageSurface {

public:
    StageSurface(Output *output, Device *device);
    ~StageSurface();

    DXGI_MAPPED_RECT map();
    void unmap();

    LONG width = 0;
    LONG height = 0;
    DXGI_FORMAT dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
    D3D11_TEXTURE2D_DESC desc;
    ID3D11Texture2D *texture = nullptr;
    Output *output = nullptr;
    Device *device = nullptr;
};

}  // namespace DXCam

#endif  //DXCAM_CPP_STAGESURFACE_H
