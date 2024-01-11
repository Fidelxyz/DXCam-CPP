#include "core/OutputMetadata.h"

namespace DXCam {

AdapterMetadata OutputMetadata::get(
        const std::wstring &adapter_device_name) const {
    for (const auto &adapter: this->adapters) {
        if (adapter.device_name == adapter_device_name) { return adapter; }
    }
    return {};
}

}  // namespace DXCam
