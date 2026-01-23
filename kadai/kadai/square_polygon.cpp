#include "square_polygon.h"
#include <cassert>
#include <DirectXMath.h>

namespace
{
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };
}

SquarePolygon::~SquarePolygon()
{
    if (vertexBuffer_) { vertexBuffer_->Release(); vertexBuffer_ = nullptr; }
    if (IndexBuffer_) { IndexBuffer_->Release(); IndexBuffer_ = nullptr; }
}

bool SquarePolygon::create(const Device& device) noexcept
{
    if (!createVertexBuffer(device)) return false;
    if (!createIndexBuffer(device)) return false;
    return true;
}

// 頂点バッファの生成 (修正版)
bool SquarePolygon::createVertexBuffer(const Device& device) noexcept
{
    Vertex squareVertices[] = {
        {{ 0.2f, -0.2f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 右下 (0)
        {{ 0.2f,  0.2f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 右上 (1)
        {{-0.2f, -0.2f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 左下 (2)
        {{-0.2f,  0.2f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}  // 左上 (3)
    };

    const auto vertexBufferSize = sizeof(squareVertices);

    // ヒープ設定 (UPLOADタイプが一般的です)
    D3D12_HEAP_PROPERTIES heapProperty{};
    heapProperty.Type = D3D12_HEAP_TYPE_UPLOAD; // シンプルにUPLOADでOK

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = vertexBufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    auto res = device.get()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer_));
    if (FAILED(res)) return false;

    Vertex* data{};
    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&data));
    memcpy(data, squareVertices, vertexBufferSize);
    vertexBuffer_->Unmap(0, nullptr);

    // --- ここが抜けていました ---
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = (UINT)vertexBufferSize;
    vertexBufferView_.StrideInBytes = sizeof(Vertex);

    return true;
}

// インデックスバッファの生成 (新規実装)
bool SquarePolygon::createIndexBuffer(const Device& device) noexcept
{
    // 0,1,2 と 2,1,3 の三角形2つで四角形を作る
    uint16_t indices[] = { 0, 1, 2, 2, 1, 3 };
    const auto indexBufferSize = sizeof(indices);

    D3D12_HEAP_PROPERTIES heapProperty{ D3D12_HEAP_TYPE_UPLOAD };
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = indexBufferSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    auto res = device.get()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&IndexBuffer_));
    if (FAILED(res)) return false;