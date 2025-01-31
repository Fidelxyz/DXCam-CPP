#include "DXFactory.h"

#include <cstdio>

#include "util/io.h"

namespace DXCam {

void DXFactory::init() {
    if (is_initialized_) return;

    DPI_AWARENESS_CONTEXT old_dpi_awareness = SetThreadDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    const auto p_adapters = enum_dxgi_adapters();
    for (const auto &p_adapter: p_adapters) {
        auto device = Device(p_adapter);
        const auto p_outputs = device.enum_outputs();
        if (!p_outputs.empty()) {
            devices.emplace_back(device);
            outputs.emplace_back(p_outputs.begin(), p_outputs.end());
            // in-place construct std::vector<Output>(p_outputs.begin(),
            // p_outputs.end())
        }
    }
    output_metadata = get_output_metadata();

    SetThreadDpiAwarenessContext(old_dpi_awareness);

    is_initialized_ = true;
}

std::shared_ptr<DXCamera> DXFactory::create(const int device_idx,
                                            int output_idx,
                                            const size_t max_buffer_len) {
    init();

    if (output_idx == -1) { output_idx = find_primary_output_idx(device_idx); }

    auto instant_ptr = find_instant(device_idx, output_idx);
    if (instant_ptr) {
        printf("You already created a DXCamera Instance for Device %d--Output "
               "%d!\n",
               device_idx, output_idx);
        return instant_ptr;
    }

    auto const device = &devices[device_idx];
    auto const output = &outputs[device_idx][output_idx];

    output->update_desc();

    long width, height;
    output->get_resolution(&width, &height);
    const Region region = {0, 0, width, height};

    auto camera = std::make_shared<DXCamera>(output, device, region, false,
                                             max_buffer_len);
    camera_instants_[std::make_tuple(device_idx, output_idx)] = camera;
    std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
    return camera;
}

std::shared_ptr<DXCamera> DXFactory::create(const Region &region,
                                            const int device_idx,
                                            int output_idx,
                                            const size_t max_buffer_len) {
    init();

    if (output_idx == -1) { output_idx = find_primary_output_idx(device_idx); }

    auto instant_ptr = find_instant(device_idx, output_idx);
    if (instant_ptr) {
        printf("You already created a DXCamera Instance for Device %d--Output "
               "%d!\n",
               device_idx, output_idx);
        return instant_ptr;
    }

    auto const device = &devices[device_idx];
    auto const output = &outputs[device_idx][output_idx];

    output->update_desc();

    auto camera = std::make_shared<DXCamera>(output, device, region, true,
                                             max_buffer_len);
    camera_instants_[std::make_tuple(device_idx, output_idx)] = camera;
    std::this_thread::sleep_for(std::chrono::nanoseconds(100000));
    return camera;
}

std::vector<DeviceInfo> DXFactory::get_devices_info() {
    init();

    std::vector<DeviceInfo> devices_info;
    for (const auto &device: devices) {
        devices_info.emplace_back(device.get_info());
    }
    return devices_info;
}

std::vector<std::vector<OutputInfo>> DXFactory::get_outputs_info() {
    init();

    std::vector<std::vector<OutputInfo>> outputs_info;
    for (const auto &device_outputs: outputs) {
        std::vector<OutputInfo> device_outputs_info;
        for (const auto &output: device_outputs) {
            OutputInfo output_info = output.get_info();
            output_info.is_primary =
                    output_metadata.get(output.get_device_name()).is_primary;
            device_outputs_info.emplace_back(std::move(output_info));
        }
        outputs_info.emplace_back(std::move(device_outputs_info));
    }
    return outputs_info;
}

int DXFactory::find_primary_output_idx(const int device_idx) const {
    for (int output_idx = 0; output_idx < output_metadata.adapters.size();
         output_idx++) {
        if (output_metadata
                    .get(outputs[device_idx][output_idx].get_device_name())
                    .is_primary) {
            return output_idx;
        }
    }
    return -1;
}

std::shared_ptr<DXCamera> DXFactory::find_instant(const int device_idx,
                                                  const int output_idx) {
    auto instant_key = std::make_tuple(device_idx, output_idx);
    decltype(camera_instants_)::iterator instant;
    while ((instant = camera_instants_.find(instant_key)) !=
           camera_instants_.end()) {
        // found existing instance not expired
        if (!instant->second.expired()) return instant->second.lock();

        // found existing instance expired
        camera_instants_.erase(instant);
    }
    return nullptr;
}

}  // namespace DXCam