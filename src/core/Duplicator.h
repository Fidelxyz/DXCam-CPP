#ifndef DXCAM_CPP_DUPLICATOR_H
#define DXCAM_CPP_DUPLICATOR_H

#include "Device.h"
#include "Output.h"

namespace DXCam {

class Duplicator {
   public:
    Duplicator(const Output *output, const Device *device);
    ~Duplicator();

    // Disallow copy and move
    Duplicator(const Duplicator &) = delete;
    Duplicator &operator=(const Duplicator &) = delete;
    Duplicator(Duplicator &&other) = delete;
    Duplicator &operator=(Duplicator &&other) = delete;

    void rebuild(const Output *output, const Device *device);

    bool update_frame();
    void release_frame() const;

    ID3D11Texture2D *texture = nullptr;
    bool updated = false;

   private:
    void create(const Output *output, const Device *device);
    void release();

    IDXGIOutputDuplication *duplicator_ = nullptr;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DUPLICATOR_H
