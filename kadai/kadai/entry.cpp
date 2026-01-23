//#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:WinMainCRTStartup")

#include "window.h" 

#include "device.h"
#include "DXGI.h"
#include "command_allocator.h"
#include "command_list.h"
#include "command_queue.h"
#include "swap_chain.h"
#include "descriptor_heap.h"
#include "render_target.h"
#include "fence.h"
#include "root_signature.h"
#include "shader.h"
#include "pipline_state_object.h"

// 両方をインクルード
#include "triangle_polygon.h"
#include "square_polygon.h" 

#include <cassert>
#include <d3d12.h>
#include "camera.h"
#include "square_polygon.h"
#include "object.h"
#include "constant_buffer.h"

class Application final {
public:
    Application() = default;
    ~Application() = default;

    [[nodiscard]] bool initialize(HINSTANCE instance) noexcept {
        if (S_OK != windowInstance_.create(instance, 1280, 720, "MyApp")) return false;
        if (!dxgiInstance_.setDisplayAdapter()) return false;
        if (!deviceInstance_.create(dxgiInstance_)) return false;
        if (!commandQueueInstance_.create(deviceInstance_)) return false;
        if (!swapChainInstance_.create(dxgiInstance_, windowInstance_, commandQueueInstance_)) return false;

        if (!descriptorHeapInstance_.create(deviceInstance_, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, swapChainInstance_.getDesc().BufferCount)) return false;
        if (!renderTargetInstance_.createBackBuffer(deviceInstance_, swapChainInstance_, descriptorHeapInstance_)) return false;

        if (!commandAllocatorInstance_[0].create(deviceInstance_, D3D12_COMMAND_LIST_TYPE_DIRECT)) return false;
        if (!commandAllocatorInstance_[1].create(deviceInstance_, D3D12_COMMAND_LIST_TYPE_DIRECT)) return false;

        if (!commandListInstance_.create(deviceInstance_, commandAllocatorInstance_[0])) return false;
        if (!fenceInstance_.create(deviceInstance_)) return false;

        // ポリゴン生成
        if (!trianglePolygonInstance_.create(deviceInstance_)) return false;
        if (!squarePolygonInstance_.create(deviceInstance_)) return false; // ここでエラーが出るなら変数名を確認

        if (!rootSignatureInstance_.create(deviceInstance_)) return false;
        if (!shaderInstance_.create(deviceInstance_)) return false;
        if (!piplineStateObjectInstance_.create(deviceInstance_, shaderInstance_, rootSignatureInstance_)) return false;

        cameraInstance_.initialize();

        // ディスクリプタヒープ (CBV用)
        if (!constantBufferDescriptorHeapInstance_.create(deviceInstance_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, true)) return false;

        // 定数バッファ作成
        if (!cameraConstantBufferInstance_.create(deviceInstance_, constantBufferDescriptorHeapInstance_, sizeof(Camera::ConstBufferData), 0)) return false;
        if (!trianglePolygonConstantBufferInstance_.create(deviceInstance_, constantBufferDescriptorHeapInstance_, sizeof(TrianglePolygon::ConstBufferData), 1)) return false;

        // 四角形用 (ここもクラス名や構造体名を確認してください)
        if (!squarePolygonConstantBufferInstance_.create(deviceInstance_, constantBufferDescriptorHeapInstance_, sizeof(SquarePolygon::ConstBufferData), 2)) return false;

        return true;
    }

    void loop() noexcept {
        while (windowInstance_.messageLoop()) {
            cameraInstance_.update();
            triangleObjectInstance_.update();
            squareObjectInstance_.update();

            const auto backBufferIndex = swapChainInstance_.get()->GetCurrentBackBufferIndex();

            if (frameFenceValue_[backBufferIndex] != 0) {
                fenceInstance_.wait(frameFenceValue_[backBufferIndex]);
            }

            commandAllocatorInstance_[backBufferIndex].reset();
            commandListInstance_.reset(commandAllocatorInstance_[backBufferIndex]);

            auto pToRT = resourceBarrier(renderTargetInstance_.get(backBufferIndex), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandListInstance_.get()->ResourceBarrier(1, &pToRT);

            D3D12_CPU_DESCRIPTOR_HANDLE handles[] = { renderTargetInstance_.getCpuDescriptorHandle(deviceInstance_, descriptorHeapInstance_, backBufferIndex) };
            commandListInstance_.get()->OMSetRenderTargets(1, handles, false, nullptr);

            const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
            commandListInstance_.get()->ClearRenderTargetView(handles[0], clearColor, 0, nullptr);

            commandListInstance_.get()->SetPipelineState(piplineStateObjectInstance_.get());
            commandListInstance_.get()->SetGraphicsRootSignature(rootSignatureInstance_.get());

            const auto [w, h] = windowInstance_.size();
            D3D12_VIEWPORT viewport{};
            viewport.TopLeftX = 0.0f; viewport.TopLeftY = 0.0f;
            viewport.Width = static_cast<float>(w); viewport.Height = static_cast<float>(h);
            viewport.MinDepth = 0.0f; viewport.MaxDepth = 1.0f;
            commandListInstance_.get()->RSSetViewports(1, &viewport);

            D3D12_RECT scissorRect{};
            scissorRect.left = 0; scissorRect.top = 0;
            scissorRect.right = w; scissorRect.bottom = h;
            commandListInstance_.get()->RSSetScissorRects(1, &scissorRect);

            ID3D12DescriptorHeap* p[] = { constantBufferDescriptorHeapInstance_.get() };
            commandListInstance_.get()->SetDescriptorHeaps(1, p);

            // カメラ
            Camera::ConstBufferData cameraData{
                DirectX::XMMatrixTranspose(cameraInstance_.viewMatrix()),
                DirectX::XMMatrixTranspose(cameraInstance_.projection()),
            };
            UINT8* pCameraData{};
            cameraConstantBufferInstance_.constantBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&pCameraData));
            memcpy_s(pCameraData, sizeof(cameraData), &cameraData, sizeof(cameraData));
            cameraConstantBufferInstance_.constantBuffer()->Unmap(0, nullptr);
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(0, cameraConstantBufferInstance_.getGpuDescriptorHandle());

            // 三角形
            TrianglePolygon::ConstBufferData triangleData{
                DirectX::XMMatrixTranspose(triangleObjectInstance_.world()),
                triangleObjectInstance_.color() };
            UINT8* pTriangleData{};
            trianglePolygonConstantBufferInstance_.constantBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&pTriangleData));
            memcpy_s(pTriangleData, sizeof(triangleData), &triangleData, sizeof(triangleData));
            trianglePolygonConstantBufferInstance_.constantBuffer()->Unmap(0, nullptr);
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(1, trianglePolygonConstantBufferInstance_.getGpuDescriptorHandle());
            trianglePolygonInstance_.draw(commandListInstance_);

            // 四角形
            SquarePolygon::ConstBufferData squareData{
                DirectX::XMMatrixTranspose(squareObjectInstance_.world()),
                squareObjectInstance_.color()
            };
            UINT8* pSquareData{};
            squarePolygonConstantBufferInstance_.constantBuffer()->Map(0, nullptr, reinterpret_cast<void**>(&pSquareData));
            memcpy_s(pSquareData, sizeof(squareData), &squareData, sizeof(squareData));
            squarePolygonConstantBufferInstance_.constantBuffer()->Unmap(0, nullptr);
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(1, squarePolygonConstantBufferInstance_.getGpuDescriptorHandle());
            squarePolygonInstance_.draw(commandListInstance_.get());

            auto rtToP = resourceBarrier(renderTargetInstance_.get(backBufferIndex), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            commandListInstance_.get()->ResourceBarrier(1, &rtToP);

            commandListInstance_.get()->Close();

            ID3D12CommandList* ppCommandLists[] = { commandListInstance_.get() };
            commandQueueInstance_.get()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

            swapChainInstance_.get()->Present(1, 0);

            commandQueueInstance_.get()->Signal(fenceInstance_.get(), nextFenceValue_);
            frameFenceValue_[backBufferIndex] = nextFenceValue_;
            nextFenceValue_++;
        }
    }

    D3D12_RESOURCE_BARRIER resourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) noexcept {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = resource;
        barrier.Transition.StateBefore = from;
        barrier.Transition.StateAfter = to;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        return barrier;
    }

private:
    Window             windowInstance_{};
    DXGI               dxgiInstance_{};
    Device             deviceInstance_{};
    CommandQueue       commandQueueInstance_{};
    SwapChain          swapChainInstance_{};
    DescriptorHeap     descriptorHeapInstance_{};
    RenderTarget       renderTargetInstance_{};
    CommandAllocator   commandAllocatorInstance_[2]{};
    CommandList        commandListInstance_{};
    Fence              fenceInstance_{};
    UINT64             frameFenceValue_[2]{};
    UINT64             nextFenceValue_ = 1;

    RootSignature      rootSignatureInstance_{};
    Shader             shaderInstance_{};
    PiplineStateObject piplineStateObjectInstance_{};
    DescriptorHeap     constantBufferDescriptorHeapInstance_{};

    TrianglePolygon    trianglePolygonInstance_{};
    Object             triangleObjectInstance_{};
    ConstantBuffer     trianglePolygonConstantBufferInstance_{};

    // クラス名が QuadPolygon なのか SquarePolygon なのか注意してください
    // ここではあなたの最後のコードに合わせて SquarePolygon にしています
    SquarePolygon      squarePolygonInstance_{};
    Object             squareObjectInstance_{};
    ConstantBuffer     squarePolygonConstantBufferInstance_{};

    Camera             cameraInstance_{};
    ConstantBuffer     cameraConstantBufferInstance_{};
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Application app;
    if (!app.initialize(hInstance)) return -1;
    app.loop();
    return 0;
}

#if defined(_DEBUG) || defined(DEBUG)
int main() {
    // コンソール(main)から、ウィンドウ(WinMain)を無理やり呼び出す
    return WinMain(GetModuleHandle(nullptr), nullptr, nullptr, SW_SHOW);
}
#endif
