#include "StageSurface.h"

#include <cassert>

namespace DXCam {

StageSurface::StageSurface(const Output *const output,
                           const Device *const device) {
    create(output, device);
}

StageSurface::~StageSurface() { release(); }

void StageSurface::create(const Output *const output,
                          const Device *const device) {
    output->get_surface_size(&width_, &height_);
    const D3D11_TEXTURE2D_DESC desc{static_cast<UINT>(width_),
                                    static_cast<UINT>(height_),
                                    1,
                                    1,
                                    dxgi_format_,
                                    {1, 0},
                                    D3D11_USAGE_STAGING,
                                    0,
                                    D3D11_CPU_ACCESS_READ,
                                    0};

    HRESULT hr = device->device->CreateTexture2D(&desc, nullptr, &texture);
    assert(SUCCEEDED(hr));
    assert(texture != nullptr);

    hr = texture->QueryInterface(__uuidof(IDXGISurface),
                                 reinterpret_cast<void **>(&surface_));
    assert(SUCCEEDED(hr));
}

void StageSurface::release() {
    if (texture != nullptr) {
        texture->Release();
        texture = nullptr;
    }
}

void StageSurface::rebuild(const Output *output, const Device *device) {
    release();
    create(output, device);
}

DXGI_MAPPED_RECT StageSurface::map() const {
    DXGI_MAPPED_RECT rect;
    HRESULT hr = surface_->Map(&rect, DXGI_MAP_READ);
    assert(SUCCEEDED(hr));

    return rect;
}

void StageSurface::unmap() const {
    HRESULT hr = surface_->Unmap();
    assert(SUCCEEDED(hr));
}

}  // namespace DXCam