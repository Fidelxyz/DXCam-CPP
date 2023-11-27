//
// Created by Fidel on 2023/11/26.
//
#include "py_dxcam.h"

std::string device_info() {
    const auto info = DXCam::get_devices_info();
    std::string ret;
    for (size_t i = 0; i < info.size(); i++) {
        ret += std::format(
                "Device[{}]:<Device Name:{} Dedicated VRAM:{}Mb VendorId:{}>\n",
                i, wstring_to_string(info[i].description),
                info[i].vram_size / 1048576, info[i].vendor_id);
    }
    return ret;
}

std::string output_info() {
    const auto info = DXCam::get_outputs_info();
    std::string ret;
    for (size_t device_idx = 0; device_idx < info.size(); device_idx++) {
        for (size_t output_idx = 0; output_idx < info[device_idx].size();
             output_idx++) {
            const auto &output = info[device_idx][output_idx];
            ret += std::format(
                    "Device[{}] Output[{}]: Res:({}, {}) Rot:{} Primary:{}\n",
                    device_idx, output_idx, output.width, output.height,
                    output.rotation_angle,
                    output.is_primary ? "True" : "False");
        }
    }
    return ret;
}

std::shared_ptr<DXCamera> create(int device_idx,
                                 const std::optional<int> output_idx,
                                 const std::optional<py::tuple> &region,
                                 const std::string &output_color,
                                 size_t max_buffer_len) {
    if (!region) {
        return std::make_shared<DXCamera>(
                DXCam::create(device_idx, output_idx.value_or(-1),
                              max_buffer_len),
                output_color);
    }

    return std::make_shared<DXCamera>(
            DXCam::create(
                    std::make_from_tuple<DXCam::Region>(
                            py::cast<std::tuple<int, int, int, int>>(*region)),
                    device_idx, output_idx.value_or(-1), max_buffer_len),
            output_color);
}