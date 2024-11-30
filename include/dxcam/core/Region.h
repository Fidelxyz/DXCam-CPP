#ifndef DXCAM_CPP_REGION_H
#define DXCAM_CPP_REGION_H

namespace DXCam {

class Region {
public:
    int left, top, right, bottom;

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_REGION_H
