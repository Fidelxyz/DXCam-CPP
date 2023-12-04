//
// Created by Fidel on 2023/10/26.
//

#ifndef DXCAM_CPP_REGION_H
#define DXCAM_CPP_REGION_H

#include <compare>

namespace DXCam {

class Region {
public:
    int left, top, right, bottom;

    [[nodiscard]] int get_width() const;
    [[nodiscard]] int get_height() const;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_REGION_H
