//
// Created by Fidel on 2023/10/27.
//

#include "DXFactory.h"

#include <cstdio>

#include "util/io.h"

namespace DXCam {

DXFactory::DXFactory() {
    const auto p_adapters = enum_dxgi_adapters();
    for (const auto &p_adapter: p_adapters) {
        auto device = Device(p_adapter);
        const auto p_outputs = device.enum_outputs();
        if (!p_outputs.empty()) {
            this->devices.emplace_back(device);
            this->outputs.emplace_back(p_outputs.begin(), p_outputs.end());
            // in-place construct std::vector<Output>(p_outputs.begin(), p_outputs.end())
        }
    }
    this->output_metadata = get_output_metadata();
}

std::shared_ptr<DXCamera> DXFactory::create(const int device_idx,
                                            int output_idx,
                                            const ColorFormat output_color,
                                            const size_t max_buffer_len) {
    if (output_idx == -1) {
        output_idx = this->find_primary_output_idx(device_idx);
    }

    auto instant_ptr = find_instant(device_idx, output_idx);
    if (!instant_ptr) {
        printf("You already created a DXCamera Instance for Device %d--Output "
               "%d!\n",
               device_idx, output_idx);
        return instant_ptr;
    }

    auto const device = &this->devices[device_idx];
    auto const output = &this->outputs[device_idx][output_idx];

    output->update_desc();

    long width, height;
    output->get_resolution(&width, &height);
    const Region region = {0, 0, width, height};

    auto camera = std::make_shared<DXCamera>(output, device, region, false,
                                             output_color, max_buffer_len);
    this->camera_instants[std::make_tuple(device_idx, output_idx)] = camera;
    return camera;
}

std::shared_ptr<DXCamera> DXFactory::create(const Region &region,
                                            const int device_idx,
                                            int output_idx,
                                            const ColorFormat output_color,
                                            const size_t max_buffer_len) {
    if (output_idx == -1) {
        output_idx = this->find_primary_output_idx(device_idx);
    }

    auto instant_ptr = find_instant(device_idx, output_idx);
    if (instant_ptr) {
        printf("You already created a DXCamera Instance for Device %d--Output "
               "%d!\n",
               device_idx, output_idx);
        return instant_ptr;
    }

    auto const device = &this->devices[device_idx];
    auto const output = &this->outputs[device_idx][output_idx];

    output->update_desc();

    auto camera = std::make_shared<DXCamera>(output, device, region, true,
                                             output_color, max_buffer_len);
    this->camera_instants[std::make_tuple(device_idx, output_idx)] = camera;
    return camera;
}

std::vector<DeviceInfo> DXFactory::get_devices_info() const {
    std::vector<DeviceInfo> devices_info;
    for (const auto &device: this->devices) {
        devices_info.emplace_back(device.get_info());
    }
    return devices_info;
}

std::vector<std::vector<OutputInfo>> DXFactory::get_outputs_info() const {
    std::vector<std::vector<OutputInfo>> outputs_info;
    for (const auto &device_outputs: this->outputs) {
        std::vector<OutputInfo> device_outputs_info;
        for (const auto &output: device_outputs) {
            device_outputs_info.emplace_back(output.get_info());
        }
        outputs_info.emplace_back(std::move(device_outputs_info));
    }
    return outputs_info;
}

int DXFactory::find_primary_output_idx(const int device_idx) const {
    for (int output_idx = 0; output_idx < this->output_metadata.adapters.size();
         output_idx++) {
        if (this->output_metadata
                    .get(this->outputs[device_idx][output_idx]
                                 .get_device_name())
                    .is_primary) {
            return output_idx;
        }
    }
    return -1;
}

std::shared_ptr<DXCamera> DXFactory::find_instant(const int device_idx,
                                                  const int output_idx) {
    auto instant_key = std::make_tuple(device_idx, output_idx);
    decltype(this->camera_instants)::iterator instant;
    while ((instant = this->camera_instants.find(instant_key)) !=
           this->camera_instants.end()) {
        // found existing instance not expired
        if (!instant->second.expired()) { return instant->second.lock(); }

        // found existing instance expired
        this->camera_instants.erase(instant);
    }
    return nullptr;
}

}  // namespace DXCam