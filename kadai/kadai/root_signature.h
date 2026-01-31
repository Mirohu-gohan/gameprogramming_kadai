#pragma once

#include "device.h"

class RootSignature final {
public:
    RootSignature() = default;
    ~RootSignature();

    [[nodiscard]] bool create(const Device& device) noexcept;
    [[nodiscard]] ID3D12RootSignature* get() const noexcept;

private:
    ID3D12RootSignature* rootSignature_{};
};