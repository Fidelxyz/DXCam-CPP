#include "numpy.h"

py::array_t<uint8_t> numpy_array_from(const cv::Mat &&mat) {
    const auto mat_ = new cv::Mat(std::move(mat));  // ?

    const py::capsule capsule(
        mat_, [](void *data) { delete static_cast<cv::Mat *>(data); });

    if (mat_->isContinuous()) {
        return py::array_t<uint8_t>({mat_->rows, mat_->cols, mat_->channels()},
                                    mat_->data, capsule);
    } else {
        return py::array_t<uint8_t>(
            {mat_->rows, mat_->cols, mat_->channels()},
            {mat_->step[0], mat_->step[1], static_cast<size_t>(1)}, mat_->data,
            capsule);
    }
}
