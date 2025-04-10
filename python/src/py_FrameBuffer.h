#ifndef DXCAM_CPP_PYTHON_PY_FRAMEBUFFER_H
#define DXCAM_CPP_PYTHON_PY_FRAMEBUFFER_H

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "dxcam/DXCamera.h"

namespace py = pybind11;

class FrameBuffer {
   public:
    explicit FrameBuffer(const std::shared_ptr<DXCam::DXCamera> &camera);

    std::vector<py::array_t<uint8_t>> enter();
    void exit(const py::object &type, const py::object &value,
              const py::object &traceback) const;

   private:
    std::shared_ptr<DXCam::DXCamera> camera_;
    std::mutex *mutex_ = nullptr;
};

#endif
