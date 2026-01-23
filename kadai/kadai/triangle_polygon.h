// 三角形ポリゴンクラス

#pragma once

#include "device.h"
#include "command_list.h"
#include <DirectXMath.h>

//---------------------------------------------------------------------------------
/**
 * @brief	ポリゴンクラス
 */
class TrianglePolygon final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファ用データ構造体
     */
    struct ConstBufferData {
        DirectX::XMMATRIX world_{};  /// ワールド行列
        DirectX::XMFLOAT4 color_{};  /// カラー(RGBA)
    };

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    TrianglePolygon() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~TrianglePolygon();

    //---------------------------------------------------------------------------------
    /**
     * @brief	ポリゴンの生成
     * @param	device	デバイスクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool create(const Device& device) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ポリゴンの描画
     * @param	commandList	コマンドリスト
     */
    void draw(const CommandList& commandList) noexcept;

private:
    //---------------------------------------------------------------------------------
    /**
     * @brief	頂点バッファの生成
     * @param	device	デバイスクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool createVertexBuffer(const Device& device) noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	インデックスバッファの生成
     * @param	device	デバイスクラスのインスタンス
     * @return	成功すれば true
     */
    [[nodiscard]] bool createIndexBuffer(const Device& device) noexcept;

private:
    ID3D12Resource* vertexBuffer_{};  /// 頂点バッファ
    ID3D12Resource* indexBuffer_{};   /// インデックスバッファ

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};  ///< 頂点バッファビュー
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_ = {};  ///< インデックスバッファビュー
};

