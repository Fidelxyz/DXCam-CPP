#include "py_DXCamera.h"

#include <utility>

#include "util/numpy.h"

const std::unordered_map<std::string, cv::ColorConversionCodes>
    DXCamera::cvt_color_flag_map_ = {
        {"RGB", cv::COLOR_BGRA2RGB},      {"RGBA", cv::COLOR_BGRA2RGBA},
        {"BGR", cv::COLOR_BGRA2BGR},      {"GRAY", cv::COLOR_BGRA2GRAY},
        {"BGRA", cv::COLOR_COLORCVT_MAX},  // no conversion
};

DXCamera::DXCamera(std::shared_ptr<DXCam::DXCamera> &&camera,
                   const std::string &output_color)
    : camera_(std::move(camera)),
      cvt_color_flag_(cvt_color_flag_map_.at(output_color)) {}

void DXCamera::release() {
    assert(camera_.use_count() == 1);
    camera_.reset();
}

std::optional<py::array_t<uint8_t>> DXCamera::grab(
    const std::optional<py::tuple> &region) const {
    cv::Mat frame;
    if (!region) {
        frame = camera_->grab();
    } else {
        const auto region_ = std::make_from_tuple<DXCam::Region>(
            py::cast<std::tuple<int, int, int, int>>(*region));
        frame = camera_->grab(region_);
    }

    if (frame.empty()) { return std::nullopt; }

    if (cvt_color_flag_ != cv::COLOR_COLORCVT_MAX) {
        cv::cvtColor(frame, frame, cvt_color_flag_);
    }

    return numpy_array_from(std::move(frame));
}

void DXCamera::start(const std::optional<py::tuple> &region,
                     const int target_fps, const bool video_mode,
                     const int delay) const {
    if (!region) {
        camera_->start(target_fps, video_mode, delay);
    } else {
        const auto region_ = std::make_from_tuple<DXCam::Region>(
            py::cast<std::tuple<int, int, int, int>>(*region));
        camera_->start(region_, target_fps, video_mode, delay);
    }
}

void DXCamera::stop() const { camera_->stop(); }

py::array_t<uint8_t> DXCamera::get_latest_frame() const {
    cv::Mat frame = camera_->get_latest_frame();

    if (cvt_color_flag_ != cv::COLOR_COLORCVT_MAX) {
        cv::cvtColor(frame, frame, cvt_color_flag_);
    }

    return numpy_array_from(std::move(frame));
}

FrameBuffer DXCamera::frame_buffer() const { return FrameBuffer(camera_); };

int DXCamera::get_width() const { return camera_->get_width(); }

int DXCamera::get_height() const { return camera_->get_height(); }

int DXCamera::get_channel_size() const {
    switch (cvt_color_flag_) {
        case cv::COLOR_BGRA2RGB:
        case cv::COLOR_BGRA2BGR:
            return 3;
        case cv::COLOR_BGRA2RGBA:
        case cv::COLOR_COLORCVT_MAX:  // BGRA
            return 4;
        case cv::COLOR_BGRA2GRAY:
            return 1;
        default:
            assert(false);
            return 0;
    }
}

int DXCamera::get_rotation_angle() const {
    return camera_->get_rotation_angle();
}

py::tuple DXCamera::get_region() const {
    const auto &[left, top, right, bottom] = camera_->get_region();
    return py::make_tuple(left, right, top, bottom);
}

size_t DXCamera::get_max_buffer_len() const {
    return camera_->get_buffer_len();
}

bool DXCamera::is_capturing() const { return camera_->is_capturing(); }
