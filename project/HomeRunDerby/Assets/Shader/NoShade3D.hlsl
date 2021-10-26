Texture2D		g_texture : register(t0);	//テクスチャー
SamplerState	g_sampler : register(s0);	//サンプラー

//コンスタントバッファ
cbuffer global
{
	float4x4	g_matVertexConversion;	//頂点変換行列
	float4x4	g_matNormalConversion;	//法線変換行列
	float4x4	g_matWorld;				//ワールド行列

	float4		g_diffuseColor;			//マテリアルの色
	float4		g_ambient;				//環境光
	float4		g_specular;				//鏡面反射光

	float4		g_camPos;				//カメラの位置
	float		g_shininess;			//光沢率
	bool		g_isTexture;			//テクスチャの有無
};

//vs -> psの入出力構造体
struct VS_OUT
{
	float4 position	: SV_POSITION;	//位置
	float4 normal	: NORMAL;		//法線
	float4 eye		: TEXCOORD;		//視線
	float2 uv		: TEXCOORD1;	//UV座標
};

//頂点シェーダ
VS_OUT VS(float4 position : POSITION, float2 uv : TEXCOORD, float4 normal : NORMAL)
{
	VS_OUT outData;															//出力データ
	outData.position	= mul(position, g_matVertexConversion);				//頂点の座標を頂点変換行列で変換する
	outData.normal		= mul(normal, g_matNormalConversion);				//頂点の法線を法線変換用行列で変換する
	outData.eye			= normalize(g_camPos - mul(position, g_matWorld));	//頂点から視点へのベクトル
	outData.uv			= uv;												//テクスチャの貼り付けに使用する座標

	return 	outData;
}

//ピクセルシェーダ
float4 PS(VS_OUT inData) : SV_Target
{
	//テクスチャの有無を確認して描画ピクセル上のオブジェクトの色を設定する
	float4 diffuse;																	//オブジェクトの色
	if (g_isTexture == true) diffuse = g_texture.Sample(g_sampler, inData.uv);		//テクスチャを使用していればテクスチャ色
	else					 diffuse = g_diffuseColor;								//使用していなければマテリアル色

	return diffuse;
}