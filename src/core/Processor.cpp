//
// Created by Fidel on 2023/10/26.
//

#include "core/Processor.h"

namespace DXCam {

cv::Mat Processor::process(const DXGI_MAPPED_RECT &rect, int width, int height,
                           const Region &region, const int rotation_angle) {
    auto pitch = rect.Pitch;

    // Pre-crop
    int offset = 0;
    switch (rotation_angle) {
        case 0:
            offset = region.top * pitch;
            height = region.bottom - region.top;
            break;
        case 90:
            offset = (width - region.right) * pitch;
            width = region.right - region.left;
            break;
        case 180:
            offset = (height - region.bottom) * pitch;
            height = region.bottom - region.top;
            break;
        case 270:
            offset = region.left * pitch;
            width = region.right - region.left;
            break;
        default:
            assert(false);  // never reach
            break;
    }

    pitch /= 4;  // number of channels
    cv::Mat image;
    if (rotation_angle == 0 || rotation_angle == 180) {
        image = cv::Mat(height, pitch, CV_8UC4, rect.pBits + offset);
    } else {
        image = cv::Mat(width, pitch, CV_8UC4, rect.pBits + offset);
    }

    // Rotation
    switch (rotation_angle) {
        case 0:
            break;
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
            assert(false);  // never reach
            break;
    }

    // Crop
    if (region.right - region.left != width) {
        image = image.colRange(region.left, region.right);
    } else if (region.bottom - region.top != height) {
        image = image.rowRange(region.top, region.bottom);
    }

    return image;
}

}  // namespace DXCam