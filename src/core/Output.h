#ifndef DXCAM_CPP_OUTPUT_H
#define DXCAM_CPP_OUTPUT_H

#include <dxgi.h>
#include <dxgi1_2.h>

#include <string>

#include "../../include/dxcam/core/OutputInfo.h"

namespace DXCam {

class Output {
   public:
    explicit Output(IDXGIOutput1 *output);

    [[nodiscard]] OutputInfo get_info() const;
    [[nodiscard]] std::wstring get_device_name() const;
    void get_resolution(LONG *width, LONG *height) const;
    void get_surface_size(LONG *width, LONG *height) const;
    [[nodiscard]] int get_rotation_angle() const;

    void update_desc();

    IDXGIOutput1 *output;

   private:
    constexpr static int ROTATION_MAPPING[] = {0, 0, 90, 180, 270};

    DXGI_OUTPUT_DESC desc_;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_OUTPUT_H
