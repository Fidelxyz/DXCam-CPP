//
// Created by Fidel on 2023/11/12.
//
#include <pybind11/pybind11.h>

#include "dxcam.h"
#include "util/string.h"

namespace py = pybind11;

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

PYBIND11_MODULE(dxcam_cpp, m) {
    m.doc() = "DXCam-CPP warpped in Python";

    m.def("device_info", &device_info, "List all outputs.");
    m.def("output_info", &output_info, "List all outputs.");
}