#ifndef DXCAM_CPP_DEVICEINFO_H
#define DXCAM_CPP_DEVICEINFO_H

#include <Windows.h>
#include <string>

namespace DXCam {

/**
 * @brief The information of a device, i.e. graphics card.
 */
struct DeviceInfo {
    /**
     * @brief The description of the device.
     */
    [[maybe_unused]] std::wstring description;

    /**
     * @brief The size of the VRAM in MiB.
     */
    [[maybe_unused]] SIZE_T vram_size;

    /**
     * @brief The vendor ID of the device.
     */
    [[maybe_unused]] UINT vendor_id;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_DEVICEINFO_H
