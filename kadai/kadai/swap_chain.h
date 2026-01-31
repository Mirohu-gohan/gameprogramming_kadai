#pragma once

#include "DXGI.h"
#include "command_queue.h"
#include "Window.h"

class SwapChain final {
public:
    SwapChain() = default;
    ~SwapChain();

    [[nodiscard]] bool create(const DXGI& dxgi, const Window& window, const CommandQueue& commandQueue) noexcept;
    [[nodiscard]] IDXGISwapChain3* get() const noexcept;
    [[nodiscard]] const DXGI_SWAP_CHAIN_DESC1& getDesc() const noexcept;

private:
    IDXGISwapChain3* swapChain_{};
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
};