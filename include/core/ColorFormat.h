//
// Created by Fidel on 2023/10/30.
//

#ifndef DXCAM_CPP_COLORFORMAT_H
#define DXCAM_CPP_COLORFORMAT_H

#include <map>

namespace DXCam {

enum ColorFormat { RGB, RGBA, BGR, BGRA, GRAY };

const static std::map<ColorFormat, size_t> CHANNEL_SIZE = {
        {RGB,  3},
        {RGBA, 4},
        {BGR,  3},
        {BGRA, 4},
        {GRAY, 1}
};

}  // namespace DXCam

#endif  //DXCAM_CPP_COLORFORMAT_H
