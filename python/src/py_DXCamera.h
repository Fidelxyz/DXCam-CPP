//
// Created by Fidel on 2023/11/16.
//

#ifndef DXCAM_CPP_PY_DXCAMERA_H
#define DXCAM_CPP_PY_DXCAMERA_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <unordered_map>

#include "DXCamera.h"

namespace py = pybind11;

class DXCamera {
public:
    DXCamera(std::shared_ptr<DXCam::DXCamera> &&camera,
             const std::string &output_color);

    py::array_t<uint8_t> grab();

private:
    std::shared_ptr<DXCam::DXCamera> camera;
    static std::unordered_map<void *, cv::Mat *> frame_map;

    cv::ColorConversionCodes cvt_color_flag;
    const static std::unordered_map<std::string, cv::ColorConversionCodes>
            cvt_color_flag_map;
};

#endif  // DXCAM_CPP_PY_DXCAMERA_H
