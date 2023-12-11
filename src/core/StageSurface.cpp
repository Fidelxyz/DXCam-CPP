//
// Created by Fidel on 2023/10/26.
//

#include "core/StageSurface.h"

#include <cassert>

namespace DXCam {

StageSurface::StageSurface(Output *const output, Device *const device)
    : desc() {
    output->get_surface_size(&this->width, &this->height);
    this->desc = {
            static_cast<UINT>(this->width),
            static_cast<UINT>(this->height),
            1,
            1,
            this->dxgi_format,
            {1, 0},
            D3D11_USAGE_STAGING,
            0,
            D3D11_CPU_ACCESS_READ,
            0
    };

    HRESULT hr = device->device->CreateTexture2D(&this->desc, nullptr,
                                                 &this->texture);
    assert(SUCCEEDED(hr));

    hr = this->texture->QueryInterface(__uuidof(IDXGISurface),
                                       reinterpret_cast<void **>(&surface));
    assert(SUCCEEDED(hr));
}

StageSurface::~StageSurface() {
    if (this->texture != nullptr) {
        this->texture->Release();
        this->texture = nullptr;
    }
}

DXGI_MAPPED_RECT StageSurface::map() const {
    DXGI_MAPPED_RECT rect;
    HRESULT hr = surface->Map(&rect, DXGI_MAP_READ);
    assert(SUCCEEDED(hr));

    return rect;
}

void StageSurface::unmap() const { surface->Unmap(); }

}  // namespace DXCam