#include "py_FrameBuffer.h"

#include "util/numpy.h"

FrameBuffer::FrameBuffer(const std::shared_ptr<DXCam::DXCamera> &camera)
    : camera_(camera) {}

std::vector<py::array_t<uint8_t>> FrameBuffer::enter() {
    const cv::Mat *const *frame_buffer;
    const std::atomic_int *head, *tail;
    const size_t *len;

    camera_->get_frame_buffer(&frame_buffer, nullptr, &len, &head, &tail,
                              nullptr, &mutex_);

    mutex_->lock();

    std::vector<py::array_t<uint8_t>> frames;
    for (size_t i = *head; i != *tail; i = (i + 1) % *len) {
        cv::Mat frame = (*frame_buffer)[i];
        frames.push_back(numpy_array_from(std::move(frame)));
    }

    return frames;
}

void FrameBuffer::exit([[maybe_unused]] const py::object &type,
                       [[maybe_unused]] const py::object &value,
                       [[maybe_unused]] const py::object &traceback) const {
    if (mutex_ != nullptr) { mutex_->unlock(); }
}
