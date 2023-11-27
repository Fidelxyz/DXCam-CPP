//
// Created by Fidel on 2023/11/12.
//
#ifndef DXCAM_CPP_PY_DXCAM_H
#define DXCAM_CPP_PY_DXCAM_H

#include <pybind11/pybind11.h>

#include "dxcam.h"
#include "py_DXCamera.h"
#include "util/string.h"

namespace py = pybind11;

std::string device_info();
std::string output_info();

std::shared_ptr<DXCamera> create(int device_idx, std::optional<int> output_idx,
                                 const std::optional<py::tuple> &region,
                                 const std::string &output_color,
                                 size_t max_buffer_len);

#endif  // DXCAM_CPP_PY_DXCAM_H