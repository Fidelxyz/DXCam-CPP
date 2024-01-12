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

    [[nodiscard]] DXGI_MAPPED_RECT map() const;
    void unmap() const;

    ID3D11Texture2D *texture = nullptr;

private:
    LONG width_ = 0;
    LONG height_ = 0;
    DXGI_FORMAT dxgi_format_ = DXGI_FORMAT_B8G8R8A8_UNORM;
    D3D11_TEXTURE2D_DESC desc_;
    IDXGISurface *surface_ = nullptr;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_STAGESURFACE_H
