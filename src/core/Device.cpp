//
// Created by Fidel on 2023/10/26.
//

#include "core/Device.h"

namespace DXCam {

Device::Device(IDXGIAdapter1 *const adapter) : adapter(adapter), desc() {
    this->adapter->GetDesc1(&this->desc);

    const D3D_FEATURE_LEVEL feature_levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
    };

    D3D11CreateDevice(this->adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
                      feature_levels, std::size(feature_levels), 7,
                      &this->device, nullptr, &this->context);
    this->device->GetImmediateContext(&this->im_context);
}

std::vector<IDXGIOutput *> Device::enum_outputs() const {
    std::vector<IDXGIOutput *> p_outputs;
    IDXGIOutput *p_output;
    for (size_t i = 0;
         this->adapter->EnumOutputs(i, &p_output) != DXGI_ERROR_NOT_FOUND;
         i++) {
        p_outputs.emplace_back(p_output);
    }
    return p_outputs;
}

DeviceInfo Device::get_info() const {
    return {this->desc.Description, this->desc.DedicatedVideoMemory,
            this->desc.VendorId};
}

}  // namespace DXCam