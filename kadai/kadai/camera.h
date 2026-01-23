#pragma once// カメラ制御クラス

#pragma once

#include <DirectXMath.h>

//---------------------------------------------------------------------------------
/**
 * @brief	カメラ制御クラス
 */
class Camera final {
public:
    //---------------------------------------------------------------------------------
    /**
     * @brief	コンスタントバッファ用データ構造体
     */
    struct ConstBufferData {
        DirectX::XMMATRIX view_{};        /// ビュー行列
        DirectX::XMMATRIX projection_{};  /// 射影行列
    };

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    コンストラクタ
     */
    Camera() = default;

    //---------------------------------------------------------------------------------
    /**
     * @brief    デストラクタ
     */
    ~Camera() = default;

public:
    //---------------------------------------------------------------------------------
    /**
     * @brief    カメラを初期化する
     */
    void initialize() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief    カメラを更新する
     */
    void update() noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief   カメラのビュー行列を取得する
     * @return	ビュー行列
     */
    [[nodiscard]] DirectX::XMMATRIX XM_CALLCONV viewMatrix() const noexcept;

    //---------------------------------------------------------------------------------
    /**
     * @brief   プロジェクション行列を取得する
     * @return	プロジェクション行列
     */
    [[nodiscard]] DirectX::XMMATRIX XM_CALLCONV projection() const noexcept;

private:
    DirectX::XMMATRIX view_{};        /// ビュー行列
    DirectX::XMMATRIX projection_{};  /// 射影行列

    // カメラのパラメータ
    DirectX::XMFLOAT3 position_{};  /// カメラの位置
    DirectX::XMFLOAT3 target_{};    /// カメラの注視点
    DirectX::XMFLOAT3 up_{};        /// カメラの上方向
};
