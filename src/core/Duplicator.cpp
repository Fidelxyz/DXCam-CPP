#include "core/Duplicator.h"

#include <cassert>
#include <iostream>

namespace DXCam {

Duplicator::Duplicator(Output *const output, Device *const device) {
    HRESULT hr = output->output->DuplicateOutput(device->device, &duplicator_);
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
    if (duplicator_ != nullptr) {
        duplicator_->Release();
        duplicator_ = nullptr;
    }
}

bool Duplicator::update_frame() {
    DXGI_OUTDUPL_FRAME_INFO info;
    IDXGIResource *res = nullptr;
    HRESULT hr = duplicator_->AcquireNextFrame(0, &info, &res);
    if (hr == DXGI_ERROR_ACCESS_LOST) { return false; }
    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        updated = false;
        return true;
    }
    assert(SUCCEEDED(hr));

    hr = res->QueryInterface(__uuidof(ID3D11Texture2D),
                             reinterpret_cast<void **>(&texture));
    if (FAILED(hr)) { duplicator_->ReleaseFrame(); }

    updated = true;
    return true;
}

void Duplicator::release_frame() const {
    HRESULT hr = duplicator_->ReleaseFrame();
    assert(SUCCEEDED(hr) || hr == DXGI_ERROR_INVALID_CALL);
}

}  // namespace DXCam