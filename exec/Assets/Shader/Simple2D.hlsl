Texture2D		g_texture : register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー

//コンスタントバッファ
cbuffer global
{
	float4x4 g_matVertexConversion;			//頂点座標変換行列
};

//vs -> psの入出力構造体
struct VS_OUT
{
	float4 position : SV_POSITION;	//位置
	float2 uv		: TEXCOORD;   	//UV座標
};

//頂点シェーダ
VS_OUT VS(float4 position : POSITION, float2 uv : TEXCOORD)
{
	//ピクセルシェーダーへ渡す情報を設定
	VS_OUT outData;
	outData.position = mul(position, g_matVertexConversion);
	outData.uv		 = uv;

	//まとめて出力
	return outData;
}

// ピクセルシェーダ
float4 PS(VS_OUT inData) : SV_Target
{
	return g_texture.Sample(g_sampler, inData.uv);
}