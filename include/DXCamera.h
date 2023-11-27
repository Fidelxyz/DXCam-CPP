#ifndef DXCAM_CPP_DXCAMERA_H
#define DXCAM_CPP_DXCAMERA_H

#include <array>
#include <string>
#include <thread>

#include "core/Device.h"
#include "core/Duplicator.h"
#include "core/Output.h"
#include "core/Processor.h"
#include "core/Region.h"
#include "core/StageSurface.h"
#include "dxcam_export.h"

namespace DXCam {

class DXCamera {
public:
    DXCamera(Output *output, Device *device, const Region &region,
             bool region_set_by_user, size_t max_buffer_len = 64);
    DXCAM_EXPORT ~DXCamera();

    DXCamera(const DXCamera &) = delete;  // disable copy constructor

    /**
     * @brief Capture the default region instantly.
     *
     * @return a Mat object, containing the captured image.
     */
    [[maybe_unused]] DXCAM_EXPORT cv::Mat grab();
    /**
     * @brief Capture the specified region instantly.
     *
     * @param [in] region The rectangle region to be captured.
     * @return a Mat object, containing the captured image.
     */
    [[maybe_unused]] DXCAM_EXPORT cv::Mat grab(const Region &region);

    /**
     * @brief Start capturing the default region.
     *
     * @param [in] target_fps The target fps.
     * @param [in] video_mode If true, a frame will always be pushed to the
     * frame buffer at the target fps even if there is no a new frame available.
     * If false, only new frames will be pushed to the frame buffer.
     * @param [in] delay The delay in seconds before capturing starts.
     */
    [[maybe_unused]] DXCAM_EXPORT void start(int target_fps = 60,
                                             bool video_mode = false,
                                             int delay = 0);
    /**
     * @brief Start capturing the specified region.
     *
     * @param [in] region The rectangle region to be captured.
     * @param [in] target_fps The target fps.
     * @param [in] video_mode If true, a frame will always be pushed to the
     * frame buffer at the target fps even if there is no a new frame available.
     * If false, only new frames will be pushed to the frame buffer.
     * @param [in] delay The delay in seconds before capturing starts.
     */
    [[maybe_unused]] DXCAM_EXPORT void start(const Region &region,
                                             int target_fps = 60,
                                             bool video_mode = false,
                                             int delay = 0);

    /**
     * @brief Stop capturing.
     */
    [[maybe_unused]] DXCAM_EXPORT void stop();

    /**
     * @brief Get the latest frame in the frame buffer.
     *
     * @return a Mat object, containing the latest frame in the frame buffer.
     */
    [[maybe_unused]] DXCAM_EXPORT cv::Mat get_latest_frame();

    /**
     * @brief Get the pointers to access the frame buffer, which is a circular
     * queue.
     *
     * If you want to access the whole frame buffer, you should lock
     * frame_buffer_all_mutex. If you want to access a single frame, you should
     * lock the corresponding mutex in frame_buffer_mutex.
     *
     * @param [out] frame_buffer The pointer to (the pointer to the first
     * element of) the frame buffer array.
     * @param [out] frame_buffer_mutex The pointer to (the pointer to the first
     * element of) the array of the mutex of single frames.
     * @param [out] len The pointer to the length of the frame buffer.
     * @param [out] head The pointer to the index of the oldest frame in the
     * frame buffer.
     * @param [out] tail The pointer to the index of the next frame of the
     * latest frame in the frame buffer.
     * @param [out] full The pointer to the flag indicating whether the frame
     * buffer is full.
     * @param [out] frame_buffer_all_mutex The pointer to the mutex of the frame
     * buffer.
     */
    [[maybe_unused]] DXCAM_EXPORT void get_frame_buffer(
            const cv::Mat *const **frame_buffer,
            std::mutex *const **frame_buffer_mutex, const size_t **len,
            const std::atomic<int> **head, const std::atomic<int> **tail,
            const std::atomic<bool> **full,
            std::mutex **frame_buffer_all_mutex);

    long width = 0;
    long height = 0;
    int rotation_angle;
    size_t max_buffer_len;
    bool is_capturing = false;

private:
    void validate_region(const Region &region) const;

    void capture(const Region &region, int target_fps = 60,
                 bool video_mode = false);

    void on_output_change();

    void rebuild_frame_buffer(const Region &region);

    Output *output;
    Device *device;
    StageSurface stagesurf;
    Duplicator duplicator;
    Processor processor;

    Region region;
    bool region_set_by_user;

    std::mutex frame_buffer_all_mutex;
    std::mutex *frame_buffer_mutex = nullptr;
    std::atomic<bool> frame_available = false;
    cv::Mat *frame_buffer = nullptr;
    std::atomic<int> head = 0;
    std::atomic<int> tail = 0;
    std::atomic<bool> full = false;

    std::thread thread;
    std::atomic<bool> stop_capture = false;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DXCAMERA_H
