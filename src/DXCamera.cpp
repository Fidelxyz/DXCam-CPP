#include "DXCamera.h"

#include <comdef.h>

#include <cstdio>
#include <format>
#include <stdexcept>
#include <thread>

namespace DXCam {
DXCamera::DXCamera(Output *const output, Device *const device,
                   const Region &region, const bool region_set_by_user,
                   const ColorFormat output_color, const size_t max_buffer_len)
    : output(output),
      device(device),
      region(region),
      region_set_by_user(region_set_by_user),
      stagesurf(output, device),
      duplicator(output, device),
      processor(output_color),
      max_buffer_len(max_buffer_len) {
    this->output->get_resolution(&this->width, &this->height);
    this->validate_region(this->region);

    this->channel_size = CHANNEL_SIZE.at(output_color);
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
    this->thread = std::thread([this, &region, &target_fps, &video_mode] {
        this->capture(region, target_fps, video_mode);
    });
}

void DXCamera::stop() {
    if (this->is_capturing) {
        this->frame_available = true;
        this->frame_available_cv.notify_all();
        this->stop_capture = true;
        if (this->thread.joinable()) { this->thread.join(); }
        this->is_capturing = false;
    }
    delete[] this->frame_buffer.data();
}

cv::Mat DXCamera::get_latest_frame() {
    std::unique_lock lock(this->frame_buffer_mutex);
    this->frame_available_cv.wait(lock,
                                  [this] { return this->frame_available; });
    this->frame_available = false;
    return this->frame_buffer[(this->head - 1) % this->max_buffer_len];
}

void DXCamera::capture(const Region &region, const int target_fps,
                       const bool video_mode) {
    if (target_fps <= 0) {
        throw std::invalid_argument("Target FPS should be greater than 0");
    }

    const auto period = std::chrono::duration<double>(1.0 / target_fps);
    auto last_frame_time =
            std::chrono::time_point<std::chrono::high_resolution_clock>::min();

    // for FPS statistics
    const auto capture_start_time = std::chrono::high_resolution_clock::now();
    int frame_count = 0;

    while (!this->stop_capture) {
        std::this_thread::sleep_until(last_frame_time + period);
        last_frame_time = std::chrono::high_resolution_clock::now();

        auto frame = this->grab(region);

        if (!frame.empty() || video_mode) {
            std::scoped_lock lock(this->frame_buffer_mutex);

            if (video_mode && frame.empty()) {
                frame = this->frame_buffer[(this->tail - 1) %
                                           static_cast<int>(max_buffer_len)];
            }

            this->frame_buffer[this->tail] = std::move(frame);
            this->tail =
                    (this->tail + 1) % static_cast<int>(this->max_buffer_len);
            if (this->full) { this->head = this->tail; }
            this->full = this->head == this->tail;

            this->frame_available = true;
            frame_count++;
        }
    }

    const auto capture_stop_time = std::chrono::high_resolution_clock::now();
    const auto capture_duration = duration_cast<std::chrono::seconds>(
            capture_stop_time - capture_start_time);
    const int fps = static_cast<int>(
            static_cast<double>(frame_count) /
            static_cast<double>(
                    std::chrono::seconds(capture_duration).count()));
    printf("Screen Capture FPS: %d\n", fps);
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

    while (true) {
        try {
            this->stagesurf = StageSurface(this->output, this->device);
            this->duplicator = Duplicator(this->output, this->device);
        } catch (const _com_error &e) {  // TODO untested
            continue;
        }
        break;
    }
}

void DXCamera::rebuild_frame_buffer() {
    this->rebuild_frame_buffer(this->region);
}

void DXCamera::rebuild_frame_buffer(const Region &region) {
    auto region_width = region.get_width();
    auto region_height = region.get_height();

    {
        std::scoped_lock lock(this->frame_buffer_mutex);
        delete[] this->frame_buffer.data();
        this->frame_buffer = std::span(new cv::Mat[this->max_buffer_len],
                                       this->max_buffer_len);
        for (auto &frame: this->frame_buffer) {
            frame.create(region_height, region_width,
                         CV_8UC(this->channel_size));
        }
        this->head = 0;
        this->tail = 0;
        this->full = false;
    }
}

void DXCamera::get_frame_buffer(const std::span<cv::Mat> *frame_buffer,
                                const int *head, const int *tail,
                                const size_t *len, const bool *full,
                                std::mutex *frame_buffer_mutex) {
    frame_buffer = &this->frame_buffer;
    head = &this->head;
    tail = &this->tail;
    len = &this->max_buffer_len;
    full = &this->full;
    frame_buffer_mutex = &this->frame_buffer_mutex;
}

}  // namespace DXCam