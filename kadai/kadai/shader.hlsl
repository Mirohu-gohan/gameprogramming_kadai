// 頂点バッファの内容をそのまま出力するシェーダ

// 頂点シェーダの入力構造体
struct VSInput
{
    float3 position : POSITION; // 入力：頂点座標
    float4 color : COLOR; // 入力：頂点色
};

// カメラコンスタントバッファ
cbuffer ConstantBuffer : register(b0)
{
    matrix view;
    matrix projection;
};

// ポリゴンコンスタントバッファ
cbuffer ConstantBuffer : register(b1)
{
    matrix world;
    float4 color;
};


// 頂点シェーダの出力構造体
struct VSOutput
{
    float4 position : SV_POSITION; // 出力：変換後座標
    float4 color : COLOR; // 出力：頂点色
};


// -------------------------------
// 頂点シェーダ
// -------------------------------
VSOutput vs(VSInput input)
{
    VSOutput output;
    
    // 3D座標を4D同次座標に変換
    float4 pos = float4(input.position, 1.0f);
	
    pos = mul(pos, world); // ポリゴンのワールド行列でワールド変換	
    pos = mul(pos, view); // カメラのビュー行列でビュー変換
    pos = mul(pos, projection); // カメラのプロジェクション行列でプロジェクション変換
	
    output.position = pos;
    
    // 色情報をそのまま次の段階に渡す
    output.color = input.color;
    
    return output;
}

// -------------------------------
// ピクセルシェーダ
// -------------------------------
float4 ps(VSOutput input) : SV_TARGET
{
	// ポリゴンの色と頂点色を乗算して出力
    return input.color * color;
}