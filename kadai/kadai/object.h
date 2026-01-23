// オブジェクトクラス

#pragma once

#include <DirectXMath.h>

//---------------------------------------------------------------------------------
/**
 * @brief	オブジェクトクラス
 */
class Object final {
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
    Object() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~Object() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief	オブジェクトの更新
     */
    void update() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	ワールド行列の取得
     * @return  ワールド行列
     */
    [[nodiscard]] DirectX::XMMATRIX world() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief	カラーの取得
     * @return  カラー
     */
    [[nodiscard]] DirectX::XMFLOAT4 color() const noexcept;

private:
    DirectX::XMMATRIX world_ = DirectX::XMMatrixIdentity();                /// ワールド行列
    DirectX::XMFLOAT4 color_ = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);  /// カラー(RGBA)

    float move_{};  /// 移動用変数
};