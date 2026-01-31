#include "command_queue.h"
#include <cassert>

CommandQueue::~CommandQueue() {
    if (commandQueue_) {
        commandQueue_->Release();
        commandQueue_ = nullptr;
    }
}

[[nodiscard]] bool CommandQueue::create(const Device& device) noexcept {
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    const auto hr = device.get()->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue_));
    if (FAILED(hr)) {
        assert(false && "Failed to create command queue");
        return false;
    }
    return true;
}

[[nodiscard]] ID3D12CommandQueue* CommandQueue::get() const noexcept {
    if (!commandQueue_) {
        assert(false && "Command queue not created");
        return nullptr;
    }
    return commandQueue_;
}