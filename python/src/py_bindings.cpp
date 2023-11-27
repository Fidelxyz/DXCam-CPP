//
// Created by Fidel on 2023/11/16.
//
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "py_dxcam.h"

namespace py = pybind11;

PYBIND11_MODULE(dxcam_cpp, m) {
    m.doc() = "DXCam-CPP warped in Python";

    m.def("device_info", &device_info, "List all outputs.");
    m.def("output_info", &output_info, "List all outputs.");

    m.def("create", &create, "Create a DXCamera instance.",
          py::arg("device_idx") = 0, py::arg("output_idx") = py::none(),
          py::arg("region") = py::none(), py::arg("output_color") = "RGB",
          py::arg("max_buffer_len") = 64);

    py::class_<DXCamera, std::shared_ptr<DXCamera>>(m, "DXCamera")
            .def("grab", &DXCamera::grab);
}