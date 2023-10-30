//
// Created by Fidel on 2023/10/26.
//

#include "core/Duplicator.h"

namespace DXCam {

Duplicator::Duplicator(Output *const output, Device *const device) {
    output->output->DuplicateOutput(device->device, &this->duplicator);
}

Duplicator::~Duplicator() {
    if (this->duplicator != nullptr) {
        this->duplicator->Release();
        this->duplicator = nullptr;
    }
}

bool Duplicator::update_frame() {
    DXGI_OUTDUPL_FRAME_INFO info;
    IDXGIResource *res = nullptr;
    HRESULT hr = this->duplicator->AcquireNextFrame(0, &info, &res);
    if (hr == DXGI_ERROR_ACCESS_LOST) { return false; }
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        this->updated = true;
        return true;
    }

    hr = res->QueryInterface(__uuidof(ID3D11Texture2D),
                             reinterpret_cast<void **>(&this->texture));
    if (FAILED(hr)) { this->duplicator->ReleaseFrame(); }

    this->updated = true;
    return true;
}

void Duplicator::release_frame() const { this->duplicator->ReleaseFrame(); }

}  // namespace DXCam