#include "Device.h"

#include <cassert>

namespace DXCam {

Device::Device(IDXGIAdapter1 *const adapter) : adapter(adapter), desc() {
    HRESULT hr = adapter->GetDesc1(&desc);
    assert(SUCCEEDED(hr));

    constexpr D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

    hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0,
                           feature_levels, std::size(feature_levels), 7,
                           &device, nullptr, &context);
    assert(SUCCEEDED(hr));
    device->GetImmediateContext(&im_context);
}

std::vector<IDXGIOutput1 *> Device::enum_outputs() const {
    std::vector<IDXGIOutput1 *> p_outputs;
    IDXGIOutput1 *p_output;
    for (size_t i = 0;
         adapter->EnumOutputs(i, reinterpret_cast<IDXGIOutput **>(&p_output)) !=
         DXGI_ERROR_NOT_FOUND;
         i++) {
        p_outputs.emplace_back(p_output);
    }
    return p_outputs;
}

DeviceInfo Device::get_info() const {
    return {desc.Description, desc.DedicatedVideoMemory, desc.VendorId};
}

}  // namespace DXCam