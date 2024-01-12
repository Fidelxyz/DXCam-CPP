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
    void release_frame() const;

    ID3D11Texture2D *texture = nullptr;
    bool updated = false;

private:
    IDXGIOutputDuplication *duplicator_ = nullptr;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DUPLICATOR_H
