#pragma once

#include "device.h"
#include "descriptor_heap.h"

class ConstantBuffer final {
public:
    ConstantBuffer() = default;
    ~ConstantBuffer();

    [[nodiscard]] bool create(const Device& device, const DescriptorHeap& heap, UINT bufferSize, UINT descriptorIndex) noexcept;
    [[nodiscard]] ID3D12Resource* constantBuffer() const noexcept;
    [[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE getGpuDescriptorHandle() const noexcept;

private:
    ID3D12Resource* constantBuffer_{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
};