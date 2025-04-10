#ifndef DXCAM_CPP_PYTHON_UTIL_NUMPY_H
#define DXCAM_CPP_PYTHON_UTIL_NUMPY_H

#include <pybind11/numpy.h>

#include <opencv2/opencv.hpp>

namespace py = pybind11;

py::array_t<uint8_t> numpy_array_from(const cv::Mat &&mat);

#endif
