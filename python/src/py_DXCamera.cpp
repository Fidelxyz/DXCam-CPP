#include "py_DXCamera.h"

#include <utility>

std::unordered_map<void *, cv::Mat *> DXCamera::frame_map;

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
    : camera(camera),
      cvt_color_flag(cvt_color_flag_map.at(output_color)) {}

py::array_t<uint8_t> DXCamera::grab() {
    auto frame = new cv::Mat(this->camera->grab());

    if (this->cvt_color_flag != cv::COLOR_COLORCVT_MAX) {
        cv::cvtColor(*frame, *frame, cvt_color_flag);
    }

    frame_map[frame->data] = frame;

    py::capsule free_when_done(frame->data, [](void *data) {
        const auto frame = frame_map.find(data);
        assert(frame != frame_map.end());
        delete frame->second;
        frame_map.erase(frame);
    });

    if (frame->isContinuous()) {
        return py::array_t<uint8_t>(
                {frame->rows, frame->cols, frame->channels()}, frame->data,
                free_when_done);
    } else {
        return py::array_t<uint8_t>(
                {frame->rows, frame->cols, frame->channels()},
                {frame->step[0], frame->step[1], size_t(1)}, frame->data,
                free_when_done);
    }
}
