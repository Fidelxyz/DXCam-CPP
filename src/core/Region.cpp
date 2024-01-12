#include "core/Region.h"

namespace DXCam {

int Region::get_width() const { return right - left; }

int Region::get_height() const { return bottom - top; }

}  // namespace DXCam