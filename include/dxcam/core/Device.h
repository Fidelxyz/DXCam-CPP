#ifndef DXCAM_CPP_DEVICE_H
#define DXCAM_CPP_DEVICE_H

#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include <vector>

#include "core/DeviceInfo.h"

namespace DXCam {

class Device {
public:
    explicit Device(IDXGIAdapter1 *adapter);

    [[nodiscard]] DeviceInfo get_info() const;
    [[nodiscard]] std::vector<IDXGIOutput1 *> enum_outputs() const;

    IDXGIAdapter1 *adapter;
    ID3D11Device *device = nullptr;
    ID3D11DeviceContext *context = nullptr;
    ID3D11DeviceContext *im_context = nullptr;
    DXGI_ADAPTER_DESC1 desc;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DEVICE_H
