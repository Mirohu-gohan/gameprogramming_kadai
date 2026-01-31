#pragma once

#include "device.h"

class CommandQueue final {
public:
    CommandQueue() = default;
    ~CommandQueue();

    [[nodiscard]] bool create(const Device& device) noexcept;
    [[nodiscard]] ID3D12CommandQueue* get() const noexcept;

private:
    ID3D12CommandQueue* commandQueue_{};
};