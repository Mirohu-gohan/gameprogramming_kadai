#include "render_target.h"
#include <cassert>

RenderTarget::~RenderTarget() {
    for (auto& rt : renderTargets_) {
        if (rt) {
            rt->Release();
            rt = nullptr;
        }
    }
    renderTargets_.clear();
}

[[nodiscard]] bool RenderTarget::createBackBuffer(const Device& device, const SwapChain& swapChain, const DescriptorHeap& heap) noexcept {
    const auto& desc = swapChain.getDesc();
    renderTargets_.resize(desc.BufferCount);

    auto handle = heap.get()->GetCPUDescriptorHandleForHeapStart();
    auto heapType = heap.getType();
    assert(heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (uint8_t i = 0; i < desc.BufferCount; ++i) {
        const auto hr = swapChain.get()->GetBuffer(i, IID_PPV_ARGS(&renderTargets_[i]));
        if (FAILED(hr)) return false;

        device.get()->CreateRenderTargetView(renderTargets_[i], nullptr, handle);
        handle.ptr += device.get()->GetDescriptorHandleIncrementSize(heapType);
    }
    return true;
}

[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::getCpuDescriptorHandle(const Device& device, const DescriptorHeap& heap, UINT index) const noexcept {
    auto handle = heap.get()->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * device.get()->GetDescriptorHandleIncrementSize(heap.getType());
    return handle;
}

[[nodiscard]] ID3D12Resource* RenderTarget::get(UINT index) const noexcept {
    return renderTargets_[index];
}