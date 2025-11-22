#include "Dx12.h"
#include <Windows.h> // ★ 追加: OutputDebugString, CreateEventなどのWindows APIのために必須



#pragma comment(lib, "d3d12.lib") 
#pragma comment(lib, "dxgi.lib")

// --- 既存の初期化関数 ---

IDXGIFactory4* Dx12::CreateDXGIFactory() {
    IDXGIFactory4* factory;
    UINT           createFactoryFlags = 0;

#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    HRESULT hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        // ★ 修正: TEXT()マクロを適用
        OutputDebugString(TEXT("Failed to create DXGI Factory\n"));
        return nullptr;
    }

    return factory;
}

IDXGIAdapter1* Dx12::GetHardwareAdapter(IDXGIFactory4* factory) {
    IDXGIAdapter1* adapter;
    for (UINT adapterIndex = 0;; ++adapterIndex) {
        adapter = nullptr;
        if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter)) {
            break;
        }

        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            adapter->Release();
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            return adapter;
        }

        adapter->Release();
    }
    return nullptr;
}

ID3D12Device* Dx12::CreateD3D12Device(IDXGIAdapter1* adapter) {
    ID3D12Device* device;

    HRESULT hr = D3D12CreateDevice(
        adapter,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&device)
    );

    if (FAILED(hr)) {
        hr = D3D12CreateDevice(
            nullptr,
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&device));

        if (FAILED(hr)) {
            // ★ 修正: TEXT()マクロを適用
            OutputDebugString(TEXT("Failed to create D3D12 Device\n"));
            return nullptr;
        }

        // ★ 修正: TEXT()マクロを適用
        OutputDebugString(TEXT("Using software adapter (WARP)\n"));
    }

    return device;
}

ID3D12CommandQueue* Dx12::CreateCommandQueue(ID3D12Device* device) {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0;

    ID3D12CommandQueue* commandQueue;
    HRESULT             hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

    if (FAILED(hr)) {
        // ★ 修正: TEXT()マクロを適用
        OutputDebugString(TEXT("Failed to create Command Queue\n"));
        return nullptr;
    }

    // ★ 修正: TEXT()マクロを適用
    commandQueue->SetName(TEXT("Main Command Queue"));

    return commandQueue;
}

IDXGISwapChain3* Dx12::CreateSwapChain(IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = 1280;
    swapChainDesc.Height = 720;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGISwapChain1* swapChain1;
    HRESULT          hr = factory->CreateSwapChainForHwnd(
        commandQueue,
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    );

    if (FAILED(hr)) {
        // ★ 修正: TEXT()マクロを適用
        OutputDebugString(TEXT("Failed to create Swap Chain\n"));
        return nullptr;
    }

    IDXGISwapChain3* swapChain;
    hr = swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain));
    swapChain1->Release();

    if (FAILED(hr)) {
        // ★ 修正: TEXT()マクロを適用
        OutputDebugString(TEXT("Failed to cast to SwapChain3\n"));
        return nullptr;
    }

    return swapChain;
}

void Dx12::EnableDebugLayer() {
#if defined(_DEBUG)
    ID3D12Debug* debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();

        ID3D12Debug1* debugController1;
        if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1)))) {
            debugController1->SetEnableGPUBasedValidation(TRUE);
            debugController1->Release(); // QueryInterfaceで参照カウントが増えたので解放
        }
        debugController->Release(); // D3D12GetDebugInterfaceで参照カウントが増えたので解放
    }
#endif
}

// --- 新規追加された初期化・描画処理関数 ---

bool Dx12::Initialize(HWND hwnd) {
    EnableDebugLayer();

    IDXGIFactory4* factory = CreateDXGIFactory();
    if (!factory) return false;

    IDXGIAdapter1* adapter = GetHardwareAdapter(factory);

    m_device = CreateD3D12Device(adapter);
    if (!m_device) { factory->Release(); return false; }

    m_commandQueue = CreateCommandQueue(m_device);
    if (!m_commandQueue) { factory->Release(); return false; }

    m_swapChain = CreateSwapChain(factory, m_commandQueue, hwnd);
    if (!m_swapChain) { factory->Release(); return false; }

    factory->Release();
    if (adapter) adapter->Release();

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    if (!CreateRTVHeapAndTargets()) return false;
    if (!CreateCommandObjects()) return false;
    if (!CreateFenceObjects()) return false;

    // 最初のフレームのためにGPU同期を待機 (初回はすぐ完了する)
    WaitForPreviousFrame();

    return true;
}

bool Dx12::CreateRTVHeapAndTargets() {
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    HRESULT hr = m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
    if (FAILED(hr)) return false;

    for (UINT i = 0; i < 2; i++) {
        hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        if (FAILED(hr)) return false;

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += i * m_rtvDescriptorSize;

        m_device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
    }
    return true;
}

bool Dx12::CreateCommandObjects() {
    HRESULT hr;
    for (UINT i = 0; i < 2; i++) {
        hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));
        if (FAILED(hr)) return false;
    }

    hr = m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocators[0],
        nullptr,
        IID_PPV_ARGS(&m_commandList));
    if (FAILED(hr)) return false;

    m_commandList->Close();
    return true;
}

bool Dx12::CreateFenceObjects() {
    HRESULT hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    if (FAILED(hr)) return false;

    // ★ 修正: m_fenceEventはDx12.hでnullptr初期化済み
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr) return false;

    return true;
}

void Dx12::WaitForPreviousFrame() {
    // 描画の最初で呼ばれるため、前のフレームの描画完了を待機する。

    UINT currentBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

    // GPUが処理を完了したか確認
    UINT64 completedValue = m_fence->GetCompletedValue();
    if (completedValue < m_fenceValues[currentBackBuffer]) {
        // GPUが未完了の場合、完了を待機
        m_fence->SetEventOnCompletion(m_fenceValues[currentBackBuffer], m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    // 新しいコマンド実行に備えてグローバルフェンス値をインクリメント
    m_fenceValue++;

    // コマンドキューに新しいフェンス値をシグナルするコマンドを積む
    m_commandQueue->Signal(m_fence, m_fenceValue);

    // 現在のバッファの完了値を保存
    m_fenceValues[currentBackBuffer] = m_fenceValue;
}

void Dx12::Render() {
    // 1. 同期処理 (次のフレームでこのバッファが使用可能か確認)
    WaitForPreviousFrame();

    m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

    // 2. コマンド記録の準備
    m_commandAllocators[m_currentBackBufferIndex]->Reset();
    m_commandList->Reset(m_commandAllocators[m_currentBackBufferIndex], nullptr);

    // 3. 画面クリアコマンドの記録

    // リソースバリア：Present → Render Target
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = m_renderTargets[m_currentBackBufferIndex];
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    // 画面クリア(Windowの色）
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += m_currentBackBufferIndex * m_rtvDescriptorSize;
    float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // リソースバリア：Render Target → Present
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    m_commandList->ResourceBarrier(1, &barrier);

    m_commandList->Close();

    // 4. コマンドの実行と表示
    ID3D12CommandList* ppCommandLists[] = { m_commandList };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    m_swapChain->Present(1, 0);
}

void Dx12::ReleaseAll() {
    WaitForPreviousFrame();

    if (m_commandList) m_commandList->Release();
    for (int i = 0; i < 2; ++i) {
        if (m_renderTargets[i]) m_renderTargets[i]->Release();
        if (m_commandAllocators[i]) m_commandAllocators[i]->Release();
    }
    if (m_rtvHeap) m_rtvHeap->Release();
    if (m_swapChain) m_swapChain->Release();
    if (m_commandQueue) m_commandQueue->Release();
    if (m_device) m_device->Release();
    if (m_fence) m_fence->Release();

    if (m_fenceEvent) CloseHandle(m_fenceEvent);
}