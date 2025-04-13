#ifndef DXCAM_CPP_DXCAMERA_H
#define DXCAM_CPP_DXCAMERA_H

#include <atomic>
#include <opencv2/opencv.hpp>

#include "core/Region.h"
#include "dxcam_export.h"

namespace DXCam {

class Device;
class Duplicator;
class Output;
class StageSurface;

class DXCamera {
   public:
    DXCamera(Output *output, Device *device, const Region &region,
             bool region_set_by_user, size_t max_buffer_len = 64);
    DXCAM_EXPORT ~DXCamera();

    // Disallow copy and assign
    DXCamera(const DXCamera &) = delete;
    DXCamera &operator=(const DXCamera &) = delete;

    /**
     * @brief Get the width of the region being captured.
     *
     * @return The width in pixels.
     */
    [[maybe_unused]] DXCAM_EXPORT long get_width() const;
    /**
     * @brief Get the height of the region being captured.
     *
     * @return The height in pixels.
     */
    [[maybe_unused]] DXCAM_EXPORT long get_height() const;
    /**
     * @brief Get the rotation angle of the screen being captured.
     *
     * @return The rotation angle in degrees.
     */
    [[maybe_unused]] DXCAM_EXPORT int get_rotation_angle() const;
    /**
     * @brief Get the region being captured.
     *
     * @return The region object.
     */
    [[maybe_unused]] DXCAM_EXPORT const Region &get_region() const;
    /**
     * @brief Get the size of the frame buffer in continuous capturing mode.
     *
     * @return The length of the frame buffer.
     */
    [[maybe_unused]] DXCAM_EXPORT size_t get_buffer_len() const;
    /**
     * @brief Check if the camera is currently capturing.
     *
     * @return True if capturing, false otherwise.
     */
    [[maybe_unused]] DXCAM_EXPORT bool is_capturing() const;

    /**
     * @brief Grab the default region instantly.
     *
     * @return a Mat object, containing the captured image.
     */
    [[maybe_unused]] DXCAM_EXPORT cv::Mat grab();
    /**
     * @brief Grab the specified region instantly.
     *
     * @param [in] region The rectangle region to be captured.
     * @return a Mat object, containing the captured image.
     */
    [[maybe_unused]] DXCAM_EXPORT cv::Mat grab(const Region &region);

    /**
     * @brief Start capturing the default region.
     *
     * @param [in] target_fps The target FPS.
     * @param [in] video_mode If true, a frame will always be pushed to the
     * frame buffer at the target FPS even if there is no a new frame available.
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
     * @param [in] target_fps The target FPS.
     * @param [in] video_mode If true, a frame will always be pushed to the
     * frame buffer at the target FPS even if there is no a new frame available.
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
     * @brief Get the latest frame from the frame buffer.
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
        const std::atomic<bool> **full, std::mutex **frame_buffer_all_mutex);

   private:
    void validate_region(const Region &region) const;

    void capture(const Region &region, int target_fps = 60,
                 bool video_mode = false);

    void on_output_change();

    void rebuild_frame_buffer(const Region &region);

    long width_ = 0;
    long height_ = 0;
    int rotation_angle_;
    Region region_;
    bool region_set_by_user_;
    bool is_capturing_ = false;

    Output *output_;
    Device *device_;
    StageSurface *stagesurf_;
    Duplicator *duplicator_;

    const size_t buffer_len_;
    std::mutex frame_buffer_all_mutex_;
    std::mutex *frame_buffer_mutex_ = nullptr;
    cv::Mat *frame_buffer_ = nullptr;
    std::atomic_int head_ = 0;
    std::atomic_int tail_ = 0;
    std::atomic_bool full_ = false;
    std::atomic_bool frame_available_ = false;

    std::thread thread;
    std::atomic_bool stop_capture = false;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DXCAMERA_H
