#include "square_polygon.h"
#include <cassert>

using namespace DirectX;

SquarePolygon::~SquarePolygon() {
    if (vertexBuffer_) vertexBuffer_->Release();
    if (IndexBuffer_) IndexBuffer_->Release();
}

bool SquarePolygon::create(const Device& device) noexcept {
    return createVertexBuffer(device) && createIndexBuffer(device);
}

bool SquarePolygon::createVertexBuffer(const Device& device) noexcept {
    // 左側に緑色の四角形
    struct Vertex { XMFLOAT3 pos; XMFLOAT4 color; };
    Vertex v[] = {
        {{ -0.2f, -0.3f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ -0.2f,  0.3f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ -0.8f, -0.3f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{ -0.8f,  0.3f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}
    };

    D3D12_HEAP_PROPERTIES hp{}; hp.Type = D3D12_HEAP_TYPE_UPLOAD;
    hp.CreationNodeMask = 1; hp.VisibleNodeMask = 1; // ★重要設定

    D3D12_RESOURCE_DESC rd{}; rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rd.Width = sizeof(v); rd.Height = 1; rd.DepthOrArraySize = 1; rd.MipLevels = 1; rd.SampleDesc.Count = 1; rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (FAILED(device.get()->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer_)))) return false;

    Vertex* data{}; vertexBuffer_->Map(0, nullptr, (void**)&data);
    memcpy(data, v, sizeof(v)); vertexBuffer_->Unmap(0, nullptr);

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(v); vertexBufferView_.StrideInBytes = sizeof(Vertex);
    return true;
}

bool SquarePolygon::createIndexBuffer(const Device& device) noexcept {
    uint16_t i[] = { 0, 2, 1, 1, 2, 3 };

    D3D12_HEAP_PROPERTIES hp{}; hp.Type = D3D12_HEAP_TYPE_UPLOAD;
    hp.CreationNodeMask = 1; hp.VisibleNodeMask = 1; // ★重要設定

    D3D12_RESOURCE_DESC rd{}; rd.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    rd.Width = sizeof(i); rd.Height = 1; rd.DepthOrArraySize = 1; rd.MipLevels = 1; rd.SampleDesc.Count = 1; rd.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    if (FAILED(device.get()->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&IndexBuffer_)))) return false;

    uint16_t* data{}; IndexBuffer_->Map(0, nullptr, (void**)&data);
    memcpy(data, i, sizeof(i)); IndexBuffer_->Unmap(0, nullptr);

    indexBufferView_.BufferLocation = IndexBuffer_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(i); indexBufferView_.Format = DXGI_FORMAT_R16_UINT;
    return true;
}

void SquarePolygon::draw(ID3D12GraphicsCommandList* list) const {
    list->IASetVertexBuffers(0, 1, &vertexBufferView_);
    list->IASetIndexBuffer(&indexBufferView_);
    list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    list->DrawIndexedInstanced(6, 1, 0, 0, 0);
}