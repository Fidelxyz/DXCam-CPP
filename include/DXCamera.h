#ifndef DXCAM_CPP_DXCAMERA_H
#define DXCAM_CPP_DXCAMERA_H

#include <condition_variable>
#include <span>
#include <string>
#include <thread>

#include "core/ColorFormat.h"
#include "core/Device.h"
#include "core/Duplicator.h"
#include "core/Output.h"
#include "core/Processor.h"
#include "core/Region.h"
#include "core/StageSurface.h"

namespace DXCam {

class DXCamera {
public:
    DXCamera(Output *output, Device *device, const Region &region,
             bool region_set_by_user, ColorFormat output_color = RGB,
             size_t max_buffer_len = 64);
    ~DXCamera();

    cv::Mat grab();
    cv::Mat grab(const Region &region);

    void start(int target_fps = 60, bool video_mode = false, int delay = 0);
    void start(const Region &region, int target_fps = 60,
               bool video_mode = false, int delay = 0);

    void stop();

    cv::Mat get_latest_frame();

    void get_frame_buffer(const std::span<cv::Mat> *frame_buffer,
                          const int *head, const int *tail, const size_t *len,
                          const bool *full, std::mutex *frame_buffer_mutex);

    long width = 0;
    long height = 0;
    size_t channel_size;
    int rotation_angle;
    size_t max_buffer_len;
    bool is_capturing = false;

private:
    void validate_region(const Region &region) const;

    void capture(const Region &region, int target_fps = 60,
                 bool video_mode = false);

    void on_output_change();

    void rebuild_frame_buffer();
    void rebuild_frame_buffer(const Region &region);

    Output *output;
    Device *device;
    StageSurface stagesurf;
    Duplicator duplicator;
    Processor processor;

    Region region;
    bool region_set_by_user;

    bool frame_available = false;
    std::condition_variable frame_available_cv;
    std::span<cv::Mat> frame_buffer;
    int head = 0;
    int tail = 0;
    bool full = false;
    std::thread thread;
    std::mutex frame_buffer_mutex;
    std::atomic<bool> stop_capture;
};

}  // namespace DXCam

#endif  //DXCAM_CPP_DXCAMERA_H
