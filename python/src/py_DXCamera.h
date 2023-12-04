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

    void release();

    [[nodiscard]] std::optional<py::array_t<uint8_t>> grab(
            const std::optional<py::tuple> &region) const;

    void start(const std::optional<py::tuple> &region, int target_fps,
               bool video_mode, int delay) const;
    void stop() const;
    [[nodiscard]] py::array_t<uint8_t> get_latest_frame() const;

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
    [[nodiscard]] int get_channel_size() const;
    [[nodiscard]] int get_rotation_angle() const;
    [[nodiscard]] py::tuple get_region() const;
    [[nodiscard]] size_t get_max_buffer_len() const;
    [[nodiscard]] bool is_capturing() const;

private:
    static py::array_t<uint8_t> numpy_array_from(cv::Mat &&mat);

    std::shared_ptr<DXCam::DXCamera> camera;
    const cv::ColorConversionCodes cvt_color_flag;
    const static std::unordered_map<std::string, cv::ColorConversionCodes>
            cvt_color_flag_map;
};

#endif  // DXCAM_CPP_PY_DXCAMERA_H
