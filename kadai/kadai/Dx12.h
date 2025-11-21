#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <Windows.h> // HWND, HANDLE, HRESULT などのWindows型のために追加

class Dx12 {
private:
    // --- コアオブジェクト ---
    ID3D12Device* m_device = nullptr;
    ID3D12CommandQueue* m_commandQueue = nullptr;
    IDXGISwapChain3* m_swapChain = nullptr;

    // --- 描画・同期オブジェクト (ダブルバッファリング用) ---
    ID3D12DescriptorHeap* m_rtvHeap = nullptr;
    ID3D12Resource* m_renderTargets[2] = { nullptr, nullptr };
    ID3D12CommandAllocator* m_commandAllocators[2] = { nullptr, nullptr };
    ID3D12GraphicsCommandList* m_commandList = nullptr;

    // --- 同期処理 (フェンス) ---
    ID3D12Fence* m_fence = nullptr;
    UINT64 m_fenceValue = 0;
    UINT64 m_fenceValues[2] = { 0, 0 };
    HANDLE m_fenceEvent = nullptr; // ★ 修正: メンバー変数を初期化

    // --- その他管理用変数 ---
    UINT m_rtvDescriptorSize = 0;
    UINT m_currentBackBufferIndex = 0;

public:
    Dx12() {}
    ~Dx12() { ReleaseAll(); }

    // --- 公開メソッド ---
    bool Initialize(HWND hwnd);
    void Render();
    void ReleaseAll();

    // --- 内部初期化用メソッド ---
    void EnableDebugLayer();
    IDXGIFactory4* CreateDXGIFactory();
    IDXGIAdapter1* GetHardwareAdapter(IDXGIFactory4* factory);
    ID3D12Device* CreateD3D12Device(IDXGIAdapter1* adapter);
    ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device);
    IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd);
    bool CreateCommandObjects();
    bool CreateRTVHeapAndTargets();
    bool CreateFenceObjects();
    void WaitForPreviousFrame();
};