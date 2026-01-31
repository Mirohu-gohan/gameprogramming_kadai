#include "DXGI.h"
#include <cassert>

DXGI::~DXGI() {
    if (dxgiFactory_) {
        dxgiFactory_->Release();
        dxgiFactory_ = nullptr;
    }
    if (dxgiAdapter_) {
        dxgiAdapter_->Release();
        dxgiAdapter_ = nullptr;
    }
}

[[nodiscard]] bool DXGI::setDisplayAdapter() noexcept {
#if _DEBUG
    ID3D12Debug* debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
    }
#endif

    UINT createFactoryFlags = 0;
#if _DEBUG
    createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    const auto hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory_));
    if (FAILED(hr)) {
        assert(false && "Failed to create DXGI factory");
        return false;
    }

    auto select = 0;
    IDXGIAdapter1* dxgiAdapter{};
    while (dxgiFactory_->EnumAdapters1(select, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC1 desc{};
        dxgiAdapter->GetDesc1(&desc);
        select++;

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            dxgiAdapter->Release();
            continue;
        }

        if (FAILED(D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            dxgiAdapter->Release();
            continue;
        }

        dxgiAdapter_ = dxgiAdapter;
        break;
    }

    if (!dxgiAdapter_) {
        assert(false && "Failed to find suitable adapter");
        return false;
    }

    return true;
}

[[nodiscard]] IDXGIFactory4* DXGI::factory() const noexcept {
    return dxgiFactory_;
}

[[nodiscard]] IDXGIAdapter1* DXGI::displayAdapter() const noexcept {
    return dxgiAdapter_;
}