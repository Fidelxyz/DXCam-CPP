//
// Created by Fidel on 2023/10/26.
//

#include "core/Duplicator.h"

#include <cassert>
#include <iostream>

namespace DXCam {

Duplicator::Duplicator(Output *const output, Device *const device) {
    HRESULT hr =
            output->output->DuplicateOutput(device->device, &this->duplicator);
    if (hr == DXGI_ERROR_UNSUPPORTED) {
        std::cerr
                << "IDXGIOutput1::DuplicateOutput failed: "
                   "DXGI_ERROR_UNSUPPORTED.\n"
                   "If you are running this application on a Microsoft Hybrid "
                   "system, try to run the application on the integrated GPU "
                   "instead of on the discrete GPU."
                << std::endl;
        throw std::runtime_error(
                "IDXGIOutput1::DuplicateOutput failed: "
                "DXGI_ERROR_UNSUPPORTED.");
    }
    assert(SUCCEEDED(hr));
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
    assert(SUCCEEDED(hr));

    hr = res->QueryInterface(__uuidof(ID3D11Texture2D),
                             reinterpret_cast<void **>(&this->texture));
    if (FAILED(hr)) { this->duplicator->ReleaseFrame(); }

    this->updated = true;
    return true;
}

void Duplicator::release_frame() const {
    HRESULT hr = this->duplicator->ReleaseFrame();
    assert(SUCCEEDED(hr) || hr == DXGI_ERROR_INVALID_CALL);
}

}  // namespace DXCam