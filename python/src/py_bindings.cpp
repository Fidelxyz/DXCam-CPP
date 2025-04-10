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
        .def("grab", &DXCamera::grab, py::arg("region") = py::none())
        .def("start", &DXCamera::start, py::arg("region") = py::none(),
             py::arg("target_fps") = 60, py::arg("video_mode") = false,
             py::arg("delay") = 0)
        .def("stop", &DXCamera::stop)
        .def("get_latest_frame", &DXCamera::get_latest_frame)
        .def("release", &DXCamera::release)
        .def("frame_buffer", &DXCamera::frame_buffer)
        .def_property_readonly("width", &DXCamera::get_width)
        .def_property_readonly("height", &DXCamera::get_height)
        .def_property_readonly("channel_size", &DXCamera::get_channel_size)
        .def_property_readonly("rotation_angle", &DXCamera::get_rotation_angle)
        .def_property_readonly("region", &DXCamera::get_region)
        .def_property_readonly("max_buffer_len", &DXCamera::get_max_buffer_len)
        .def_property_readonly("is_capturing", &DXCamera::is_capturing);

    py::class_<FrameBuffer>(m, "FrameBuffer")
        .def("__enter__", &FrameBuffer::enter)
        .def("__exit__", &FrameBuffer::exit);
}
