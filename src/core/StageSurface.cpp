//
// Created by Fidel on 2023/10/26.
//

#include "core/StageSurface.h"

namespace DXCam {

StageSurface::StageSurface(Output *const output, Device *const device)
    : desc() {
    output->get_surface_size(&this->width, &this->height);
    if (this->texture == nullptr) {
        this->desc.Width = this->width;
        this->desc.Height = this->height;
        this->desc.Format = this->dxgi_format;
        this->desc.MipLevels = 1;
        this->desc.ArraySize = 1;
        this->desc.SampleDesc.Count = 1;
        this->desc.SampleDesc.Quality = 0;
        this->desc.Usage = D3D11_USAGE_STAGING;
        this->desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        this->desc.MiscFlags = 0;
        this->desc.BindFlags = 0;

        device->device->CreateTexture2D(&this->desc, nullptr, &this->texture);
    }
}

StageSurface::~StageSurface() {
    if (this->texture != nullptr) {
        this->texture->Release();
        this->texture = nullptr;
    }
}

DXGI_MAPPED_RECT StageSurface::map() const {
    IDXGISurface *surface = nullptr;
    this->texture->QueryInterface(__uuidof(IDXGISurface),
                                  reinterpret_cast<void **>(&surface));

    DXGI_MAPPED_RECT rect;
    surface->Map(&rect, DXGI_MAP_READ);

    return rect;
}

void StageSurface::unmap() const {
    IDXGISurface *surface = nullptr;
    this->texture->QueryInterface(__uuidof(IDXGISurface),
                                  reinterpret_cast<void **>(&surface));

    surface->Unmap();
}

}  // namespace DXCam