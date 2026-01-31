#include "swap_chain.h"
#include <cassert>
#include "Window.h"

SwapChain::~SwapChain() {
    if (swapChain_) {
        swapChain_->Release();
        swapChain_ = nullptr;
    }
}

[[nodiscard]] bool SwapChain::create(const DXGI& dxgi, const Window& window, const CommandQueue& commandQueue) noexcept {
    const auto [w, h] = window.size();

    swapChainDesc_ = {};
    swapChainDesc_.BufferCount = 2;
    swapChainDesc_.Width = w;
    swapChainDesc_.Height = h;
    swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc_.SampleDesc.Count = 1;

    IDXGISwapChain1* tempSwapChain{};
    const auto hr = dxgi.factory()->CreateSwapChainForHwnd(commandQueue.get(), window.handle(),
        &swapChainDesc_, nullptr, nullptr, &tempSwapChain);

    if (FAILED(hr)) {
        assert(false && "Failed to create SwapChain");
        return false;
    }

    const auto hr2 = tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapChain_));
    tempSwapChain->Release();

    if (FAILED(hr2)) {
        assert(false && "Failed to upgrade SwapChain");
        return false;
    }

    return true;
}

[[nodiscard]] IDXGISwapChain3* SwapChain::get() const noexcept {
    return swapChain_;
}

[[nodiscard]] const DXGI_SWAP_CHAIN_DESC1& SwapChain::getDesc() const noexcept {
    return swapChainDesc_;
}