#include "DXCamera.h"

#include <cstdio>
#include <format>
#include <span>
#include <stdexcept>
#include <thread>

#include "util/HighResTimer.h"

namespace DXCam {
DXCamera::DXCamera(Output *const output, Device *const device,
                   const Region &region, const bool region_set_by_user,
                   const size_t max_buffer_len)
    : output(output),
      device(device),
      region(region),
      region_set_by_user(region_set_by_user),
      stagesurf(output, device),
      duplicator(output, device),
      processor(),
      max_buffer_len(max_buffer_len) {
    this->output->get_resolution(&this->width, &this->height);
    this->validate_region(this->region);

    this->rotation_angle = output->get_rotation_angle();
}

DXCamera::~DXCamera() { this->stop(); }

void DXCamera::validate_region(const Region &region) const {
    if (!((0 <= region.left && region.left < region.right &&
           region.right <= this->width) &&
          (0 <= region.top && region.top < region.bottom &&
           region.bottom <= this->height))) {
        throw std::invalid_argument(
                std::format("Invalid region: Region should be in {}x{}",
                            this->width, this->height));
    }
}

cv::Mat DXCamera::grab() { return this->grab(this->region); }

cv::Mat DXCamera::grab(const Region &region) {
    this->validate_region(region);

    if (this->duplicator.update_frame()) {
        if (!this->duplicator.updated) { return {}; }
        this->device->im_context->CopyResource(this->stagesurf.texture,
                                               this->duplicator.texture);
        this->duplicator.release_frame();
        const auto rect = this->stagesurf.map();
        auto frame = this->processor.process(rect, this->width, this->height,
                                             region, this->rotation_angle);
        this->stagesurf.unmap();
        assert(!frame.empty());
        return frame;
    } else {
        this->on_output_change();
        return {};
    }
}

void DXCamera::start(const int target_fps, const bool video_mode,
                     const int delay) {
    this->start(this->region, target_fps, video_mode, delay);
}

void DXCamera::start(const Region &region, const int target_fps,
                     const bool video_mode, const int delay) {
    if (delay != 0) {
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        this->on_output_change();
    }

    this->validate_region(region);

    this->is_capturing = true;
    this->rebuild_frame_buffer(region);
    this->thread = std::thread([this, region, target_fps, video_mode] {
        this->capture(region, target_fps, video_mode);
    });
}

void DXCamera::stop() {
    if (this->is_capturing) {
        this->frame_available = true;
        this->frame_available.notify_all();
        this->stop_capture = true;
        if (this->thread.joinable()) { this->thread.join(); }
        this->stop_capture = false;
        this->is_capturing = false;
    }
    delete[] this->frame_buffer;
    this->frame_buffer = nullptr;
}

cv::Mat DXCamera::get_latest_frame() {
    this->frame_available.wait(false);
    this->frame_available = false;

    auto frame_idx =
            (this->head - 1 + this->max_buffer_len) % this->max_buffer_len;
    std::scoped_lock lock(this->frame_buffer_mutex[frame_idx]);
    return this->frame_buffer[frame_idx];
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

    while (!this->stop_capture) {
        timer.wait();

        auto frame = this->grab(region);

        if (video_mode || !frame.empty()) {
            std::scoped_lock lock_all(this->frame_buffer_all_mutex);

            if (video_mode && frame.empty()) {
                frame = this->frame_buffer[(this->tail - 1 + max_buffer_len) %
                                           static_cast<int>(max_buffer_len)];
            }

            // The order of the following instructions is important for thread
            // safety!

            // Move the head pointer.
            // This should be done before writing the frame.
            if (this->full) {
                this->head = (this->head + 1) %
                             static_cast<int>(this->max_buffer_len);
            }
            // Now, the frame to be written will be considered outside the range
            // of contents of frame buffer.
            {
                // Lock the mutex of the single frame.
                std::scoped_lock lock_frame(
                        this->frame_buffer_mutex[this->tail]);
                this->frame_buffer[this->tail] = std::move(frame);
            }
            // Update the "full" flag.
            // This should be done before moving the tail pointer;
            // otherwise, if the tail pointer is moved but the full flag is not
            // updated, the user will get an empty frame buffer.
            this->full = this->tail + 1 == this->head;
            // Move the tail pointer.
            // This should be done after all the other operations are finished.
            // In this case, the new frame is ready to be included in the
            // contents of the frame buffer.
            this->tail =
                    (this->tail + 1) % static_cast<int>(this->max_buffer_len);

            this->frame_available = true;
            this->frame_available.notify_all();
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

    this->output->update_desc();
    this->output->get_resolution(&this->width, &this->height);
    if (this->region_set_by_user) {
        this->validate_region(this->region);
    } else {
        this->region = {0, 0, this->width, this->height};
    }

    if (this->is_capturing) { this->rebuild_frame_buffer(this->region); }

    this->rotation_angle = this->output->get_rotation_angle();

    this->stagesurf = StageSurface(this->output, this->device);
    this->duplicator = Duplicator(this->output, this->device);
}

void DXCamera::rebuild_frame_buffer(const Region &region) {
    auto region_width = region.get_width();
    auto region_height = region.get_height();

    // TODO: Check for locked mutex?

    {
        std::scoped_lock lock(this->frame_buffer_all_mutex);

        delete[] this->frame_buffer;
        this->frame_buffer = new cv::Mat[this->max_buffer_len];
        for (auto &frame: std::span(this->frame_buffer, this->max_buffer_len)) {
            frame.create(region_height, region_width, CV_8UC4);
        }
        delete[] this->frame_buffer_mutex;
        this->frame_buffer_mutex = new std::mutex[this->max_buffer_len];
        this->head = 0;
        this->tail = 0;
        this->full = false;
    }
}

void DXCamera::get_frame_buffer(const cv::Mat *const **frame_buffer,
                                std::mutex *const **frame_buffer_mutex,
                                const size_t **len,
                                const std::atomic<int> **head,
                                const std::atomic<int> **tail,
                                const std::atomic<bool> **full,
                                std::mutex **frame_buffer_all_mutex) {
    *frame_buffer = &this->frame_buffer;
    *frame_buffer_mutex = &this->frame_buffer_mutex;
    *len = &this->max_buffer_len;
    *head = &this->head;
    *tail = &this->tail;
    *full = &this->full;
    *frame_buffer_all_mutex = &this->frame_buffer_all_mutex;
}

}  // namespace DXCam