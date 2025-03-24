#ifndef DXCAM_CPP_PROCESSOR_H
#define DXCAM_CPP_PROCESSOR_H

#include <dxgi.h>

#include <opencv2/opencv.hpp>

#include "core/Region.h"

namespace DXCam {

class Processor {
   public:
    Processor() = default;

    static cv::Mat process(const DXGI_MAPPED_RECT &rect, int width, int height,
                           const Region &region, int rotation_angle);
};

}  // namespace DXCam

#endif  // DXCAM_CPP_PROCESSOR_H
