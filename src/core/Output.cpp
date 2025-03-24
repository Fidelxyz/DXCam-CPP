#include "Output.h"

#include <cassert>
#include <utility>

namespace DXCam {

Output::Output(IDXGIOutput1 *const output) : output(output), desc_() {
    update_desc();
}

void Output::update_desc() {
    HRESULT hr = output->GetDesc(&desc_);
    assert(SUCCEEDED(hr));
}

OutputInfo Output::get_info() const {
    LONG width, height;
    get_resolution(&width, &height);
    return {get_device_name(), width, height, get_rotation_angle()};
}

std::wstring Output::get_device_name() const { return desc_.DeviceName; }

void Output::get_resolution(LONG *width, LONG *height) const {
    *width = desc_.DesktopCoordinates.right - desc_.DesktopCoordinates.left;
    *height = desc_.DesktopCoordinates.bottom - desc_.DesktopCoordinates.top;
}

void Output::get_surface_size(LONG *width, LONG *height) const {
    LONG ret_width = 0;
    LONG ret_height = 0;
    get_resolution(&ret_width, &ret_height);
    if (get_rotation_angle() == 90 || get_rotation_angle() == 270) {
        std::swap(ret_width, ret_height);
    }
    *width = ret_width;
    *height = ret_height;
}

int Output::get_rotation_angle() const {
    return ROTATION_MAPPING[desc_.Rotation];
}

}  // namespace DXCam