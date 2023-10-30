//
// Created by Fidel on 2023/10/26.
//
#include "core/Region.h"

namespace DXCam {

int Region::get_width() const { return this->right - this->left; }

int Region::get_height() const { return this->bottom - this->top; }

}  // namespace DXCam