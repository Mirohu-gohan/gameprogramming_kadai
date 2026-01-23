// オブジェクトクラス

#include "object.h"
#include <cmath>

//---------------------------------------------------------------------------------
/**
 * @brief	ポリゴンの更新
 */
void Object::update() noexcept {

    move_ += 0.02f;
    world_ = DirectX::XMMatrixTranslation(0.0f, std::sinf(move_) * 1.5f, 0.0f);

    color_ = DirectX::XMFLOAT4(0.1f, 1.0f, 1.0f, 1.0f);
}

//---------------------------------------------------------------------------------
/**
 * @brief	ワールド行列の取得
 * @return  ワールド行列
 */
[[nodiscard]] DirectX::XMMATRIX Object::world() const noexcept {
    return world_;
}

//---------------------------------------------------------------------------------
/**
 * @brief	カラーの取得
 * @return  カラー
 */
[[nodiscard]] DirectX::XMFLOAT4 Object::color() const noexcept {
    return color_;
}
