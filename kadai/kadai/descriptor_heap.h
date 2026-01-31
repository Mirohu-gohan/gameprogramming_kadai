#pragma once

#include "device.h"

class DescriptorHeap final {
public:
    DescriptorHeap() = default;
    ~DescriptorHeap();

    [[nodiscard]] bool create(const Device& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible = false) noexcept;
    [[nodiscard]] ID3D12DescriptorHeap* get() const noexcept;
    [[nodiscard]] D3D12_DESCRIPTOR_HEAP_TYPE getType() const noexcept;

private:
    ID3D12DescriptorHeap* heap_{};
    D3D12_DESCRIPTOR_HEAP_TYPE type_{};
};