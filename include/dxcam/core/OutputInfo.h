#ifndef DXCAM_CPP_OUTPUTINFO_H
#define DXCAM_CPP_OUTPUTINFO_H

#include <string>

namespace DXCam {

/**
 * @brief The information of an output, i.e. screen.
 */
struct OutputInfo {
    /**
     * @brief The name of the device.
     */
    [[maybe_unused]] std::wstring device_name;

    /**
     * @brief The width of the output.
     */
    [[maybe_unused]] LONG width;

    /**
     * @brief The height of the output.
     */
    [[maybe_unused]] LONG height;

    /**
     * @brief The rotation angle of the output.
     */
    [[maybe_unused]] int rotation_angle;

    /**
     * @brief Indicates if the output is the primary display.
     */
    [[maybe_unused]] bool is_primary;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_OUTPUTINFO_H
