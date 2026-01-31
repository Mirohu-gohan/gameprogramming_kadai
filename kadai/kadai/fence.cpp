#include "fence.h"
#include <cassert>

Fence::~Fence() {
    if (fence_) {
        fence_->Release();
        fence_ = nullptr;
    }
    if (waitGpuEvent_) {
        CloseHandle(waitGpuEvent_);
    }
}

[[nodiscard]] bool Fence::create(const Device& device) noexcept {
    HRESULT hr = device.get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
    if (FAILED(hr)) {
        assert(false && "Failed to create fence");
        return false;
    }
    waitGpuEvent_ = CreateEvent(nullptr, false, false, nullptr);
    if (!waitGpuEvent_) {
        assert(false && "Failed to create event");
        return false;
    }
    return true;
}

void Fence::wait(UINT64 fenceValue) const noexcept {
    if (fence_->GetCompletedValue() < fenceValue) {
        fence_->SetEventOnCompletion(fenceValue, waitGpuEvent_);
        WaitForSingleObject(waitGpuEvent_, INFINITE);
    }
}

[[nodiscard]] ID3D12Fence* Fence::get() const noexcept {
    return fence_;
}