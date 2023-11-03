//
// Created by Fidel on 2023/10/26.
//

#include "core/Processor.h"

namespace DXCam {

cv::Mat Processor::process(const DXGI_MAPPED_RECT &rect, const int width,
                           const int height, const Region &region,
                           const int rotation_angle) {
    const auto pitch = rect.Pitch / 4;

    cv::Mat image;
    if (rotation_angle == 0 || rotation_angle == 180) {
        image = cv::Mat(height, pitch, CV_8UC4, rect.pBits);
    } else {
        image = cv::Mat(width, pitch, CV_8UC4, rect.pBits);
    }

    // TODO untested
    switch (rotation_angle) {
        case 90:
            cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
            break;
        case 180:
            cv::rotate(image, image, cv::ROTATE_180);
            break;
        case 270:
            cv::rotate(image, image, cv::ROTATE_90_COUNTERCLOCKWISE);
            break;
        default:
            break;
    }

    if (region.right - region.left != width ||
        region.bottom - region.top != height) {
        image = image(cv::Range(region.top, region.bottom),
                      cv::Range(region.left, region.right));
    }

    return image;
}

}  // namespace DXCam