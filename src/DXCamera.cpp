#include "DXCamera.h"

#include <cstdio>
#include <format>
#include <span>
#include <stdexcept>
#include <thread>

#include "core/Duplicator.h"
#include "core/Output.h"
#include "core/Processor.h"
#include "core/StageSurface.h"
#include "util/HighResTimer.h"

namespace DXCam {

DXCamera::DXCamera(Output *const output, Device *const device,
                   const Region &region, const bool region_set_by_user,
                   const size_t max_buffer_len)
    : region_(region),
      region_set_by_user_(region_set_by_user),
      buffer_len_(max_buffer_len),
      output_(output),
      device_(device),
      stagesurf_(new StageSurface(output, device)),
      duplicator_(new Duplicator(output, device)) {
    output_->get_resolution(&width_, &height_);
    validate_region(region_);

    rotation_angle_ = output->get_rotation_angle();
}

DXCamera::~DXCamera() {
    stop();
    delete stagesurf_;
    delete duplicator_;
}

long DXCamera::get_width() const { return width_; }

long DXCamera::get_height() const { return height_; }

int DXCamera::get_rotation_angle() const { return rotation_angle_; }

const Region &DXCamera::get_region() const { return region_; }

size_t DXCamera::get_buffer_len() const { return buffer_len_; }

bool DXCamera::is_capturing() const { return is_capturing_; }

void DXCamera::validate_region(const Region &region) const {
    if (!((0 <= region.left && region.left < region.right &&
           region.right <= width_) &&
          (0 <= region.top && region.top < region.bottom &&
           region.bottom <= height_))) {
        throw std::invalid_argument(std::format(
            "Invalid region: Region should be in {}x{}", width_, height_));
    }
}

cv::Mat DXCamera::grab() { return grab(region_); }

cv::Mat DXCamera::grab(const Region &region) {
    validate_region(region);

    if (duplicator_->update_frame()) {
        if (!duplicator_->updated) { return {}; }
        device_->im_context->CopyResource(stagesurf_->texture,
                                          duplicator_->texture);
        duplicator_->release_frame();
        const auto rect = stagesurf_->map();
        auto frame =
            Processor::process(rect, width_, height_, region, rotation_angle_);
        stagesurf_->unmap();
        assert(!frame.empty());
        return frame;
    } else {
        on_output_change();
        return {};
    }
}

void DXCamera::start(const int target_fps, const bool video_mode,
                     const int delay) {
    start(region_, target_fps, video_mode, delay);
}

void DXCamera::start(const Region &region, const int target_fps,
                     const bool video_mode, const int delay) {
    if (is_capturing_) { stop(); }

    if (delay != 0) {
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        on_output_change();
    }

    validate_region(region);

    is_capturing_ = true;
    rebuild_frame_buffer(region);
    thread = std::thread([this, region, target_fps, video_mode] {
        capture(region, target_fps, video_mode);
    });
}

void DXCamera::stop() {
    if (is_capturing_) {
        frame_available_ = true;
        frame_available_.notify_all();
        stop_capture = true;
        if (thread.joinable()) { thread.join(); }
        stop_capture = false;
        is_capturing_ = false;
    }
    delete[] frame_buffer_;
    frame_buffer_ = nullptr;
}

cv::Mat DXCamera::get_latest_frame() {
    frame_available_.wait(false);
    frame_available_ = false;

    const size_t frame_idx = (head_ - 1 + buffer_len_) % buffer_len_;
    std::scoped_lock lock(frame_buffer_mutex_[frame_idx]);
    return frame_buffer_[frame_idx];
}

void DXCamera::capture(const Region &region, const int target_fps,
                       const bool video_mode) {
    if (target_fps <= 0) {
        throw std::invalid_argument("Target FPS should be greater than 0");
    }

    const HighResTimer timer(static_cast<int>(1000.0 / target_fps));

    // for FPS statistics
    int frame_count = 0;
    const auto capture_start_time = std::chrono::steady_clock::now();

    while (!stop_capture) {
        timer.wait();

        auto frame = grab(region);

        if (video_mode || !frame.empty()) {
            std::scoped_lock lock_all(frame_buffer_all_mutex_);

            if (video_mode && frame.empty()) {
                frame = frame_buffer_[(tail_ - 1 + buffer_len_) %
                                      static_cast<int>(buffer_len_)];
            }

            // The order of the following instructions is important for thread
            // safety!

            // Move the head pointer.
            // This should be done before writing the frame.
            if (full_) { head_ = (head_ + 1) % static_cast<int>(buffer_len_); }
            // Now, the frame to be written will be considered outside the range
            // of contents of frame buffer.
            {
                // Lock the mutex of the single frame.
                std::scoped_lock lock_frame(frame_buffer_mutex_[tail_]);
                frame_buffer_[tail_] = std::move(frame);
            }
            // Update the "full" flag.
            // This should be done before moving the tail pointer;
            // otherwise, if the tail pointer is moved but the full flag is not
            // updated, the user will get an empty frame buffer.
            full_ = tail_ + 1 == head_;
            // Move the tail pointer.
            // This should be done after all the other operations are finished.
            // In this case, the new frame is ready to be included in the
            // contents of the frame buffer.
            tail_ = (tail_ + 1) % static_cast<int>(buffer_len_);

            frame_available_ = true;
            frame_available_.notify_all();
            frame_count++;
        }
    }

    const auto capture_stop_time = std::chrono::steady_clock::now();

    timer.cancel();

    // compute FPS statistics
    const auto capture_duration = duration_cast<std::chrono::seconds>(
        capture_stop_time - capture_start_time);
    const double fps =
        static_cast<double>(frame_count) /
        static_cast<double>(std::chrono::seconds(capture_duration).count());
    printf("Screen Capture FPS: %lf\n", fps);
}

void DXCamera::on_output_change() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 100ms

    output_->update_desc();
    output_->get_resolution(&width_, &height_);
    if (region_set_by_user_) {
        validate_region(region_);
    } else {
        region_ = {0, 0, width_, height_};
    }

    if (is_capturing_) { rebuild_frame_buffer(region_); }

    rotation_angle_ = output_->get_rotation_angle();

    stagesurf_->rebuild(output_, device_);
    duplicator_->rebuild(output_, device_);
}

void DXCamera::rebuild_frame_buffer(const Region &region) {
    const int region_width = region.get_width();
    const int region_height = region.get_height();

    // TODO: Check for locked mutex?

    {
        std::scoped_lock lock(frame_buffer_all_mutex_);

        delete[] frame_buffer_;
        frame_buffer_ = new cv::Mat[buffer_len_];
        for (auto &frame : std::span(frame_buffer_, buffer_len_)) {
            frame.create(region_height, region_width, CV_8UC4);
        }
        delete[] frame_buffer_mutex_;
        frame_buffer_mutex_ = new std::mutex[buffer_len_];
        head_ = 0;
        tail_ = 0;
        full_ = false;
    }
}

void DXCamera::get_frame_buffer(const cv::Mat *const **frame_buffer,
                                std::mutex *const **frame_buffer_mutex,
                                const size_t **len,
                                const std::atomic<int> **head,
                                const std::atomic<int> **tail,
                                const std::atomic<bool> **full,
                                std::mutex **frame_buffer_all_mutex) {
    if (frame_buffer != nullptr) *frame_buffer = &frame_buffer_;
    if (frame_buffer_mutex != nullptr)
        *frame_buffer_mutex = &frame_buffer_mutex_;
    if (len != nullptr) *len = &buffer_len_;
    if (head != nullptr) *head = &head_;
    if (tail != nullptr) *tail = &tail_;
    if (full != nullptr) *full = &full_;
    if (frame_buffer_all_mutex != nullptr)
        *frame_buffer_all_mutex = &frame_buffer_all_mutex_;
}

}  // namespace DXCam