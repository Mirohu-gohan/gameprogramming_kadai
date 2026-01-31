#include <sal.h>
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
#include "triangle_polygon.h"
#include "square_polygon.h" 
#include "camera.h"
#include "object.h"
#include "constant_buffer.h"
#include <cassert>
#include <d3d12.h>

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

        if (!trianglePolygonInstance_.create(deviceInstance_)) return false;
        if (!squarePolygonInstance_.create(deviceInstance_)) return false;

        if (!rootSignatureInstance_.create(deviceInstance_)) return false;
        if (!shaderInstance_.create(deviceInstance_)) return false;
        if (!piplineStateObjectInstance_.create(deviceInstance_, shaderInstance_, rootSignatureInstance_)) return false;

        cameraInstance_.initialize();

        if (!constantBufferDescriptorHeapInstance_.create(deviceInstance_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 3, true)) return false;

        if (!cameraConstantBufferInstance_.create(deviceInstance_, constantBufferDescriptorHeapInstance_, sizeof(Camera::ConstBufferData), 0)) return false;
        if (!trianglePolygonConstantBufferInstance_.create(deviceInstance_, constantBufferDescriptorHeapInstance_, sizeof(TrianglePolygon::ConstBufferData), 1)) return false;
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
            D3D12_VIEWPORT viewport{ 0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f, 1.0f };
            commandListInstance_.get()->RSSetViewports(1, &viewport);

            D3D12_RECT scissorRect{ 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };
            commandListInstance_.get()->RSSetScissorRects(1, &scissorRect);

            ID3D12DescriptorHeap* heaps[] = { constantBufferDescriptorHeapInstance_.get() };
            commandListInstance_.get()->SetDescriptorHeaps(1, heaps);

            // Camera
            Camera::ConstBufferData cameraData{
                DirectX::XMMatrixTranspose(cameraInstance_.viewMatrix()),
                DirectX::XMMatrixTranspose(cameraInstance_.projection()),
            };
            updateCB(cameraConstantBufferInstance_, &cameraData, sizeof(cameraData));
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(0, cameraConstantBufferInstance_.getGpuDescriptorHandle());

            // Triangle
            TrianglePolygon::ConstBufferData triangleData{
                DirectX::XMMatrixTranspose(triangleObjectInstance_.world()),
                triangleObjectInstance_.color() };
            updateCB(trianglePolygonConstantBufferInstance_, &triangleData, sizeof(triangleData));
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(1, trianglePolygonConstantBufferInstance_.getGpuDescriptorHandle());
            trianglePolygonInstance_.draw(commandListInstance_);

            // Square
            SquarePolygon::ConstBufferData squareData{
                DirectX::XMMatrixTranspose(squareObjectInstance_.world()),
                squareObjectInstance_.color()
            };
            updateCB(squarePolygonConstantBufferInstance_, &squareData, sizeof(squareData));
            commandListInstance_.get()->SetGraphicsRootDescriptorTable(1, squarePolygonConstantBufferInstance_.getGpuDescriptorHandle());
            squarePolygonInstance_.draw(commandListInstance_.get());

            auto rtToP = resourceBarrier(renderTargetInstance_.get(backBufferIndex), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            commandListInstance_.get()->ResourceBarrier(1, &rtToP);

            commandListInstance_.get()->Close();

            ID3D12CommandList* ppCommandLists[] = { commandListInstance_.get() };
            commandQueueInstance_.get()->ExecuteCommandLists(1, ppCommandLists);

            swapChainInstance_.get()->Present(1, 0);

            commandQueueInstance_.get()->Signal(fenceInstance_.get(), nextFenceValue_);
            frameFenceValue_[backBufferIndex] = nextFenceValue_;
            nextFenceValue_++;
        }
    }

private:
    void updateCB(ConstantBuffer& cb, const void* data, size_t size) {
        void* p{};
        cb.constantBuffer()->Map(0, nullptr, &p);
        memcpy(p, data, size);
        cb.constantBuffer()->Unmap(0, nullptr);
    }

    D3D12_RESOURCE_BARRIER resourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES from, D3D12_RESOURCE_STATES to) noexcept {
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource;
        barrier.Transition.StateBefore = from;
        barrier.Transition.StateAfter = to;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        return barrier;
    }

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

    SquarePolygon      squarePolygonInstance_{};
    Object             squareObjectInstance_{};
    ConstantBuffer     squarePolygonConstantBufferInstance_{};

    Camera             cameraInstance_{};
    ConstantBuffer     cameraConstantBufferInstance_{};
};



int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    Application app;
    if (!app.initialize(hInstance)) return -1;
    app.loop();
    return 0;
}

#ifdef _DEBUG
int main() {
    // 第3引数は nullptr ではなく、空の文字列 "" を渡すのが正解です
    return WinMain(GetModuleHandle(nullptr), nullptr, const_cast<LPSTR>(""), SW_SHOW);
}
#endif