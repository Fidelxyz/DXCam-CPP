#include "py_DXCamera.h"

#include <utility>

const std::unordered_map<std::string, cv::ColorConversionCodes>
        DXCamera::cvt_color_flag_map = {
                {"RGB",  cv::COLOR_BGRA2RGB    },
                {"RGBA", cv::COLOR_BGRA2RGBA   },
                {"BGR",  cv::COLOR_BGRA2BGR    },
                {"GRAY", cv::COLOR_BGRA2GRAY   },
                {"BGRA", cv::COLOR_COLORCVT_MAX}, // no conversion
};

DXCamera::DXCamera(std::shared_ptr<DXCam::DXCamera> &&camera,
                   const std::string &output_color)
    : camera(std::move(camera)),
      cvt_color_flag(cvt_color_flag_map.at(output_color)) {}

void DXCamera::release() {
    assert(this->camera.use_count() == 1);
    this->camera.reset();
}

py::array_t<uint8_t> DXCamera::numpy_array_from(cv::Mat &&mat) {
    auto mat_ = new cv::Mat(std::move(mat));  // ?

    py::capsule capsule(
            mat_, [](void *data) { delete reinterpret_cast<cv::Mat *>(data); });

    if (mat_->isContinuous()) {
        return py::array_t<uint8_t>({mat_->rows, mat_->cols, mat_->channels()},
                                    mat_->data, capsule);
    } else {
        return py::array_t<uint8_t>({mat_->rows, mat_->cols, mat_->channels()},
                                    {mat_->step[0], mat_->step[1], size_t(1)},
                                    mat_->data, capsule);
    }
}

py::array_t<uint8_t> DXCamera::grab(
        const std::optional<py::tuple> &region) const {
    cv::Mat frame;
    if (!region) {
        frame = this->camera->grab();
    } else {
        auto region_ = std::make_from_tuple<DXCam::Region>(
                py::cast<std::tuple<int, int, int, int>>(*region));
        frame = this->camera->grab(region_);
    }

    if (this->cvt_color_flag != cv::COLOR_COLORCVT_MAX) {
        cv::cvtColor(frame, frame, cvt_color_flag);
    }

    return numpy_array_from(std::move(frame));
}

void DXCamera::start(const std::optional<py::tuple> &region,
                     const int target_fps, const bool video_mode,
                     const int delay) const {
    if (!region) {
        this->camera->start(target_fps, video_mode, delay);
    } else {
        auto region_ = std::make_from_tuple<DXCam::Region>(
                py::cast<std::tuple<int, int, int, int>>(*region));
        this->camera->start(region_, target_fps, video_mode, delay);
    }
}

void DXCamera::stop() const { this->camera->stop(); }

py::array_t<uint8_t> DXCamera::get_latest_frame() const {
    return numpy_array_from(this->camera->get_latest_frame());
}

int DXCamera::get_width() const { return camera->width; }

int DXCamera::get_height() const { return camera->height; }

int DXCamera::get_channel_size() const {
    switch (this->cvt_color_flag) {
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

int DXCamera::get_rotation_angle() const { return camera->rotation_angle; }

py::tuple DXCamera::get_region() const {
    return py::make_tuple(this->camera->region.left, this->camera->region.right,
                          this->camera->region.top,
                          this->camera->region.bottom);
}

size_t DXCamera::get_max_buffer_len() const {
    return this->camera->max_buffer_len;
}

bool DXCamera::is_capturing() const { return this->camera->is_capturing; }
