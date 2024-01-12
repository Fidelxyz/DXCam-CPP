#ifndef DXCAM_CPP_DXFACTORY_H
#define DXCAM_CPP_DXFACTORY_H

#include <map>
#include <memory>

#include "DXCamera.h"
#include "core/OutputMetadata.h"

namespace DXCam {

class DXFactory {
public:
    DXFactory() = default;

    std::shared_ptr<DXCamera> create(int device_idx = 0, int output_idx = -1,
                                     size_t max_buffer_len = 64);
    std::shared_ptr<DXCamera> create(const Region &region, int device_idx = 0,
                                     int output_idx = -1,
                                     size_t max_buffer_len = 64);

    [[nodiscard]] std::vector<DeviceInfo> get_devices_info();
    [[nodiscard]] std::vector<std::vector<OutputInfo>> get_outputs_info();

    std::vector<Device> devices;
    std::vector<std::vector<Output>> outputs;
    OutputMetadata output_metadata;

private:
    void init();

    [[nodiscard]] int find_primary_output_idx(int device_idx) const;
    [[nodiscard]] std::shared_ptr<DXCamera> find_instant(int device_idx,
                                                         int output_idx);

    bool is_initialized_ = false;
    std::map<std::tuple<int, int>, std::weak_ptr<DXCamera>> camera_instants_;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DXFACTORY_H
