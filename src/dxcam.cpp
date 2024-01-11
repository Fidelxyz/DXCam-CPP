#include "dxcam.h"

#include "DXFactory.h"

namespace DXCam {

static DXFactory factory = DXFactory();

std::shared_ptr<DXCamera> create(const int device_idx, const int output_idx,
                                 const size_t max_buffer_len) {
    return factory.create(device_idx, output_idx, max_buffer_len);
}

std::shared_ptr<DXCamera> create(const Region &region, const int device_idx,
                                 const int output_idx,
                                 const size_t max_buffer_len) {
    return factory.create(region, device_idx, output_idx, max_buffer_len);
}

std::vector<DeviceInfo> get_devices_info() {
    return factory.get_devices_info();
}

std::vector<std::vector<OutputInfo>> get_outputs_info() {
    return factory.get_outputs_info();
}

}  // namespace DXCam