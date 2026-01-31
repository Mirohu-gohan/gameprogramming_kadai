#include "device.h"
#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")

Device::~Device() {
    if (device_) {
        device_->Release();
        device_ = nullptr;
    }
}

[[nodiscard]] bool Device::create(const DXGI& dxgi) noexcept {
    const auto hr = D3D12CreateDevice(dxgi.displayAdapter(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device_));
    if (FAILED(hr)) {
        assert(false && "Failed to create device");
        return false;
    }
    return true;
}

[[nodiscard]] ID3D12Device* Device::get() const noexcept {
    if (!device_) {
        assert(false && "Device not created");
        return nullptr;
    }
    return device_;
}