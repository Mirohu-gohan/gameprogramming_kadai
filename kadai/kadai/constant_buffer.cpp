#include "constant_buffer.h"
#include <cassert>

ConstantBuffer::~ConstantBuffer() {
    if (constantBuffer_) {
        constantBuffer_->Release();
        constantBuffer_ = nullptr;
    }
}

[[nodiscard]] bool ConstantBuffer::create(const Device& device, const DescriptorHeap& heap, UINT bufferSize, UINT descriptorIndex) noexcept {
    const auto size = (bufferSize + 255) & ~255;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    const auto res = device.get()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_));
    if (FAILED(res)) {
        assert(false && "Failed to create constant buffer");
        return false;
    }

    auto heapType = heap.getType();
    if (heapType != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        assert(false && "Heap type is not CBV_SRV_UAV");
        return false;
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = constantBuffer_->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = size;

    UINT cbvDescriptorSize = device.get()->GetDescriptorHandleIncrementSize(heap.getType());
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = heap.get()->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += descriptorIndex * cbvDescriptorSize;

    device.get()->CreateConstantBufferView(&cbvDesc, cpuHandle);

    gpuHandle_ = heap.get()->GetGPUDescriptorHandleForHeapStart();
    gpuHandle_.ptr += descriptorIndex * cbvDescriptorSize;

    return true;
}

[[nodiscard]] ID3D12Resource* ConstantBuffer::constantBuffer() const noexcept {
    assert(constantBuffer_ && "Constant buffer not created");
    return constantBuffer_;
}

[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE ConstantBuffer::getGpuDescriptorHandle() const noexcept {
    return gpuHandle_;
}