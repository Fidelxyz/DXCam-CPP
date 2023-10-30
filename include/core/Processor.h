//
// Created by Fidel on 2023/10/26.
//

#ifndef DXCAM_CPP_PROCESSOR_H
#define DXCAM_CPP_PROCESSOR_H

#include <dxgi.h>

#include <functional>
#include <map>
#include <string>

#include "Region.h"
#include "core/ColorFormat.h"
#include "opencv2/opencv.hpp"

namespace DXCam {

class Processor {
public:
    explicit Processor(ColorFormat output_color = BGRA);

    cv::Mat process_cvt_color(const cv::Mat &image);
    cv::Mat process(const DXGI_MAPPED_RECT &rect, int width, int height,
                    const Region &region, int rotation_angle);

    std::function<cv::Mat(const cv::Mat &)> cvt_color;
    ColorFormat color_mode;

private:
    const std::map<ColorFormat, cv::ColorConversionCodes> COLOR_MAPPING = {
            {RGB,  cv::COLOR_BGRA2RGB },
            {RGBA, cv::COLOR_BGRA2RGBA},
            {BGR,  cv::COLOR_BGRA2BGR },
            {GRAY, cv::COLOR_BGRA2GRAY}
    };
};

}  // namespace DXCam

#endif  //DXCAM_CPP_PROCESSOR_H
