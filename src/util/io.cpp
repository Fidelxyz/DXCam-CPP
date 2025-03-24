#include "util/io.h"

#include <cassert>

namespace DXCam {

std::vector<IDXGIAdapter1 *> enum_dxgi_adapters() {
    IDXGIFactory1 *dxgi_factory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1),
                                    reinterpret_cast<void **>(&dxgi_factory));
    assert(SUCCEEDED(hr));

    std::vector<IDXGIAdapter1 *> adapters;
    IDXGIAdapter1 *adapter = nullptr;
    for (UINT i = 0;
         dxgi_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND;
         i++) {
        adapters.emplace_back(adapter);
        adapter = nullptr;
    }
    return adapters;
}

OutputMetadata get_output_metadata() {
    OutputMetadata output_metadata;
    auto adapter = DISPLAY_DEVICEW();
    adapter.cb = sizeof(adapter);
    for (DWORD i = 0; EnumDisplayDevicesW(nullptr, i, &adapter,
                                          EDD_GET_DEVICE_INTERFACE_NAME);
         i++) {
        if (adapter.StateFlags & DISPLAY_DEVICE_ACTIVE) {
            const bool is_primary =
                adapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
            AdapterMetadata adapter_metadata(
                adapter.DeviceName, adapter.DeviceString, is_primary, {});

            auto display = DISPLAY_DEVICEW();
            display.cb = sizeof(display);
            for (DWORD j = 0;
                 EnumDisplayDevicesW(adapter.DeviceName, j, &display, 0); j++) {
                adapter_metadata.displays.emplace_back(display.DeviceName,
                                                       display.DeviceString);
            }

            output_metadata.adapters.emplace_back(std::move(adapter_metadata));
        }
    }
    return output_metadata;
}

}  // namespace DXCam