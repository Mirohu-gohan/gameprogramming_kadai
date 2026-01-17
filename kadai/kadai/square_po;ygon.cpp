#include "square_polygon.h"
#include <cassert>
#include <DirectXMath.h>

namespace
{
	//頂点バッファフォーマット
	struct  Vertex
	{
		DirectX::XMFLOAT3 position;//頂点座標(x,y,z)
		DirectX::XMFLOAT4 color;   //長転職(r,g,b,a)
	};

}

//デストラクタ
SquarePolygon::~SquarePolygon()
{
	//頂点バッファの解放
	if (vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = nullptr;
	}

	//インデックスバッファの解放
	if (IndexBuffer_)
	{
		IndexBuffer_->Release();
		IndexBuffer_ = nullptr;
	}
}

//ポリゴンの生成
[[nodiscard]] bool SquarePolygon::create(const Device& device)noexcept
{
	//頂点バッファの生成
	if (!createVertexBuffer(device))
	{
		return false;
	}
	//インデックスバッファの生成
	if (!createIndexBuffer(device))
	{
		return false;
	}
	return true;
}

//頂点バッファの生成
[[nodiscard]] bool SquarePolygon::createVertexBuffer(const Device& device)noexcept
{
	//四角形の頂点データ
	Vertex squareVertices[] = {
		{{0.2f,-0.2f,0.0f},{0.0f,1.0f,0.0f,1.0f}},//右下頂点（緑）
		{{0.2f,0.2f,0.0f},{0.0f,1.0f,0.0f,1.0f}},//右上頂点（緑）
		{{-0.2f,-0.2f,0.0f},{0.0f,1.0f,0.0f,1.0f}},//左下頂点（緑）
		{{-0.2f,0.2f,0.0f},{0.0f,1.0f,0.0f,1.0f}}//左上頂点（緑）
	};

	//頂点データのサイズ
	const auto vertexBufferSize = sizeof(squareVertices);

	//ヒープの設定を指定
	//CPUからアクセス可能なメモリを利用するための設定
	D3D12_HEAP_PROPERTIES heapProperty{};
	heapProperty.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperty.CreationNodeMask = 1;
	heapProperty.VisibleNodeMask = 1;


	//どんなリソースを作成するかの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Alignment = 0;
	resourceDesc.Width = vertexBufferSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//頂点バッファの生成
	auto res = device.get()->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer_));
	if (FAILED(res))
	{
		assert(false && "頂点バッファの作成に失敗");
		return false;
	}

	//頂点バッファにデータを転送する
	//CPUからアクセス可能な阿曽レスを取得
	Vertex* data{};
	//バッファをマップ（CPUからアクセス可能なからアクセス可能にする）
	//vertexBuffer_を直接利用するのではなく、dataを介して更新するイメージ
	res = vertexBuffer_->Map(0, nullptr,reinterpret_cast<void**>(&data));
	if (FAILED(res))
	{
		assert(false && "頂点バッファのマップに失敗"); 
		return false;
	}
	//頂点データをコピー
	memcpy_s(data, vertexBufferSize, squareVertices, vertexBufferSize);
	//コピーが終わったのでマップ解除（CPUからアクセス不可にする）
	vertexBuffer_->Unmap(0, nullptr);

	//頂点バッファビューの設定
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();


}
