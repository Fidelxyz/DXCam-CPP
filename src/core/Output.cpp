#include "core/Output.h"

#include <cassert>
#include <utility>

namespace DXCam {

Output::Output(IDXGIOutput1 *const output) : output(output), desc() {
    this->update_desc();
}

void Output::update_desc() {
    HRESULT hr = this->output->GetDesc(&this->desc);
    assert(SUCCEEDED(hr));
}

OutputInfo Output::get_info() const {
    LONG width, height;
    this->get_resolution(&width, &height);
    return {this->get_device_name(), width, height, this->get_rotation_angle()};
}

std::wstring Output::get_device_name() const { return this->desc.DeviceName; }

void Output::get_resolution(LONG *width, LONG *height) const {
    *width = this->desc.DesktopCoordinates.right -
             this->desc.DesktopCoordinates.left;
    *height = this->desc.DesktopCoordinates.bottom -
              this->desc.DesktopCoordinates.top;
}

void Output::get_surface_size(LONG *width, LONG *height) const {
    LONG ret_width = 0;
    LONG ret_height = 0;
    this->get_resolution(&ret_width, &ret_height);
    if (this->get_rotation_angle() == 90 || this->get_rotation_angle() == 270) {
        std::swap(ret_width, ret_height);
    }
    *width = ret_width;
    *height = ret_height;
}

int Output::get_rotation_angle() const {
    return ROTATION_MAPPING[this->desc.Rotation];
}

}  // namespace DXCam