#pragma once

#include "DXGI.h"
#include <d3d12.h>

class Device final {
public:
    Device() = default;
    ~Device();

    [[nodiscard]] bool create(const DXGI& dxgi) noexcept;
    [[nodiscard]] ID3D12Device* get() const noexcept;

private:
    ID3D12Device* device_ = nullptr;
};