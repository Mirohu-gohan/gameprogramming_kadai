#pragma once
#include "device.h"
#include "command_list.h"

//ポリゴンクラス
class SquarePolygon
{
public:
	//コンストラクタ
	SquarePolygon() = default;

	//デストラクタ
	~SquarePolygon();


	//ポリゴンの生成
	[[nodiscard]] bool create(const Device& device)noexcept;

	//ポリゴンの描画
	[[nodiscard]] bool draw(const CommandList& commandList)noexcept;

private:
	//頂点バッファの生成
	[[nodiscard]] bool createVertexBuffer(const Device& device)noexcept;

	//インデックスバッファの生成
	[[nodiscard]] bool createIndexBuffer(const Device& device)noexcept;

private:
	ID3D12Resource* vertexBuffer_{};//頂点バッファ

	ID3D12Resource* IndexBuffer_{};//インデックスバッファ

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};  ///< 頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW  indexBufferView_ = {};  ///< インデックスバッファビュー

}; 

