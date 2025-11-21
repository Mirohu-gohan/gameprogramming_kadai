#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>

class Dx12 {

private:
    //初期化で作成されるコアオブジェクト
    ID3D12Device* m_devaice = nullptr;
    ID3D12CommandQueue* m_commandQueue = nullptr;
    IDXGISwapChain3* m_swapChain = nullptr;

    //描画・同期に必要なオブジェクト
    ID3D12DescriptorHeap* m_rtvHeap = nullptr;
    ID3D12Resource* m_renderTargets[2] = { nullptr,nullptr };
    ID3D12CommandAllocator* m_commandList = nullptr;

    //同期処理(フェンス）
    ID3D12Fence* m_fence = nullptr;
    UINT64  m_fenceVale = 0;
    UINT64  m_fenceValues[2] = { 0,0 };
    HANDLE  m_fenceEvent;

    UINT  m_rtvDescriptorSize = 0;
    UINT  m_currentBackBufferIndex = 0;

public:
    // コンストラクタ
    Dx12() {}
    // デストラクタ
    ~Dx12() {}

    // --- 初期化メソッド (既存のメソッドを統合し、メンバー変数に格納) ---
    bool Initialize(HWND hwnd);
    void EnableDebugLayer();

    // --- 内部作成用メソッド (既存) ---
    IDXGIFactory4* CreateDXGIFactory();
    IDXGIAdapter1* GetHardwareAdapter(IDXGIFactory4* factory);
    ID3D12Device* CreateD3D12Device(IDXGIAdapter1* adapter);
    ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* device);
    IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd);

    // --- 画面クリアまでに必要な新規メソッド ---
    bool CreateCommandObjects();
    bool CreateRTVHeapAndTargets();
    bool CreateFenceObjects();

    void Render();
    void WaitForPreviousFrame(); // GPU同期用
    void ReleaseAll(); // リソース解放用
};

};
