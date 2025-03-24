#ifndef DXCAM_CPP_STAGESURFACE_H
#define DXCAM_CPP_STAGESURFACE_H

#include <d3d11.h>
#include <dxgi.h>

#include "Device.h"
#include "Output.h"

namespace DXCam {

class StageSurface {
   public:
    StageSurface(const Output *output, const Device *device);
    ~StageSurface();

    // Disallow copy and move
    StageSurface(const StageSurface &) = delete;
    StageSurface &operator=(const StageSurface &) = delete;
    StageSurface(StageSurface &&other) = delete;
    StageSurface &operator=(StageSurface &&other) = delete;

    void rebuild(const Output *output, const Device *device);

    [[nodiscard]] DXGI_MAPPED_RECT map() const;
    void unmap() const;

    ID3D11Texture2D *texture = nullptr;

   private:
    void create(const Output *output, const Device *device);
    void release();

    LONG width_ = 0;
    LONG height_ = 0;
    DXGI_FORMAT dxgi_format_ = DXGI_FORMAT_B8G8R8A8_UNORM;
    IDXGISurface *surface_ = nullptr;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_STAGESURFACE_H
