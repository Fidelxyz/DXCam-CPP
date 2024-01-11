#ifndef DXCAM_CPP_OUTPUTMETADATA_H
#define DXCAM_CPP_OUTPUTMETADATA_H

#include <string>
#include <vector>

namespace DXCam {

struct DisplayMetadata {
    std::wstring device_name;
    std::wstring device_string;
};

struct AdapterMetadata {
    std::wstring device_name;
    std::wstring device_string;
    bool is_primary;
    std::vector<DisplayMetadata> displays;
};

class OutputMetadata {
public:
    [[nodiscard]] AdapterMetadata get(
            const std::wstring &adapter_device_name) const;

    std::vector<AdapterMetadata> adapters;
};

}  // namespace DXCam

#endif  // DXCAM_CPP_OUTPUTMETADATA_H
