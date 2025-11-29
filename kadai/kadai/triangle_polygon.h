//三角形ポリゴンクラス
#pragma once

#include "device.h"
#include "command_list.h"

class TrianglePolygon final
{
public:
	/*
	*@brief ポリゴンクラス
	*/
	TrianglePolygon() = default;

	/*
	@brief　コンストラクタ
	*/
	~TrianglePolygon();

	/*
	@brief　ポリゴン生成
	@param　device　デバイスクラスのインスタンス
	@return　成功すれば　true
	*/
	[[nodiscard]] bool create(const Device& device)noexcept;

	/*
	* @brief ポリゴンの描画
	* @param commandList　コマンドリスト
	*/
	[[nodiscard]] void draw(const CommandList& commandList)noexcept;

private:
	/*
	* @brief インデックスバッファの生成
	* @param device デバイスクラスのインスタンス
	* @return 成功すれば true
	*/
	[[nodiscard]] bool createVertexBuffer(const Device& device)noexcept;

	/*
	* @brief インデックスバッファの作成
	* @param device デバイスクラスのインスタンス
	* @retrn 成功すれば　true
	*/
	[[nodisxard]] bool createIndexBuffer(const Device& device)noexcept;

private:
	ID3D12Resource* vertexBuffer_{}; //頂点バッファ
	ID3D12Resource* indexBuffer_{}; //インデックスバッファ

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};//頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {}; //インデックスバッファビュー
};
