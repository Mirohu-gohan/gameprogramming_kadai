#pragma once
#include "device.h"
#include <DirectXMath.h>

class SquarePolygon {
public:
    // 定数バッファ用構造体
    struct ConstBufferData {
        DirectX::XMMATRIX world;
        DirectX::XMFLOAT4 color;
    };

    SquarePolygon() = default;
    ~SquarePolygon();

    [[nodiscard]] bool create(const Device& device) noexcept;
    void draw(ID3D12GraphicsCommandList* list) const;

private:
    [[nodiscard]] bool createVertexBuffer(const Device& device) noexcept;
    [[nodiscard]] bool createIndexBuffer(const Device& device) noexcept;

    ID3D12Resource* vertexBuffer_ = nullptr;
    ID3D12Resource* indexBuffer_ = nullptr; // 小文字の i に統一
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_ = {};
};