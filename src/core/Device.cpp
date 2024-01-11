#include "core/Device.h"

#include <cassert>

namespace DXCam {

Device::Device(IDXGIAdapter1 *const adapter) : adapter(adapter), desc() {
    HRESULT hr = this->adapter->GetDesc1(&this->desc);
    assert(SUCCEEDED(hr));

    const D3D_FEATURE_LEVEL feature_levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
    };

    hr = D3D11CreateDevice(this->adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
                           feature_levels, std::size(feature_levels), 7,
                           &this->device, nullptr, &this->context);
    assert(SUCCEEDED(hr));
    this->device->GetImmediateContext(&this->im_context);
}

std::vector<IDXGIOutput1 *> Device::enum_outputs() const {
    std::vector<IDXGIOutput1 *> p_outputs;
    IDXGIOutput1 *p_output;
    for (size_t i = 0;
         this->adapter->EnumOutputs(
                 i, reinterpret_cast<IDXGIOutput **>(&p_output)) !=
         DXGI_ERROR_NOT_FOUND;
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