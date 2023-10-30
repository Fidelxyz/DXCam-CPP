//
// Created by Fidel on 2023/10/27.
//

#ifndef DXCAM_CPP_IO_H
#define DXCAM_CPP_IO_H

#include <dxgi.h>
#include <dxgi1_2.h>

#include <map>
#include <span>
#include <string>
#include <vector>

#include "core/OutputMetadata.h"

namespace DXCam {

std::vector<IDXGIAdapter1 *> enum_dxgi_adapters();

std::vector<IDXGIOutput1 *> enum_dxgi_outputs(IDXGIAdapter1 *adapter);

OutputMetadata get_output_metadata();

bool get_monitor_name_by_handle(const HMONITOR &hmonitor);

}  // namespace DXCam

#endif  //DXCAM_CPP_IO_H
