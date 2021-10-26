Texture2D		g_texture		: register(t0);	//テクスチャー
Texture2D		g_normalTexture : register(t1);	//法線マップテクスチャー
SamplerState	g_sampler		: register(s0);	//サンプラー

//コンスタントバッファ
cbuffer global
{
	float4x4	g_matVertexConversion;	//頂点変換行列
	float4x4	g_matNormalConversion;	//法線変換行列
	float4x4	g_matWorld;				//ワールド行列

	float4		g_diffuseColor;			//マテリアルの色
	float4		g_ambient;				//環境光
	float4		g_specular;				//鏡面反射光

	float4		g_cameraPosition;		//カメラの位置
	float		g_shininess;			//光沢率
	bool		g_isTexture;			//テクスチャの有無
};

//vs -> psの入出力構造体
struct VS_OUT
{
	float4 position	: SV_POSITION;	//位置
	float4 eye		: TEXCOORD2;	//視線
	float2 uv		: TEXCOORD;		//UV座標
	float4 light	: TEXCOORD1;	//ライト
};

//頂点シェーダ
VS_OUT VS(float4 pos : POSITION, float2 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent : TANGENT)
{
	//ピクセルシェーダーへ渡す情報を設定
	VS_OUT outData;											//出力データ
	outData.position	= mul(pos, g_matVertexConversion);	//頂点の座標を頂点変換行列で変換する
	outData.uv			= uv;

	//接線・従法線・法線を準備
	const float3 CROSS_NORMAL_TO_TANGENT = cross((float3)normal, (float3)tangent);											//従法線ベクトル
	float4 binormal;
	binormal.x	= CROSS_NORMAL_TO_TANGENT.x;
	binormal.y	= CROSS_NORMAL_TO_TANGENT.y;
	binormal.z	= CROSS_NORMAL_TO_TANGENT.z;
	binormal.w	= 0;
	tangent.w	= 0;
	normal.w	= 0;

	//接線・従法線・法線を法線変換行列で変換
	tangent		= mul(tangent, g_matNormalConversion);
	binormal	= mul(binormal, g_matNormalConversion);
	normal		= mul(normal, g_matNormalConversion);

	//ライトのベクトルと各線の内角をライトのベクトルとして扱う
	float4 light	= float4(0, 1.f, 0.f, 0);								//ライトのベクトル
	light			= normalize(light);										//正規化
	outData.light.x = dot(light, tangent);									//xと接線
	outData.light.y = dot(light, binormal);									//yと従法線
	outData.light.z = dot(light, normal);									//zと法線

	//視線ベクトルと各線の内角をライトのベクトルとして扱う
	float4 eye		= normalize(g_cameraPosition - mul(pos, g_matWorld));	//視線ベクトル
	outData.eye.x	= dot(eye, tangent);									//xと接線
	outData.eye.y	= dot(eye, binormal);									//yと従法線
	outData.eye.z	= dot(eye, normal);										//zと法線

	return outData;
}

//ピクセルシェーダ
float4 PS(VS_OUT inData) : SV_TARGET
{
	//ライトのベクトルを正規化
	float4 LIGHT_DIRECTION = normalize(inData.light);

	//テクスチャの使用枚数を各方向100倍する
	//拡大変換による引き伸ばし対策
	float2 uv = inData.uv * 100.f;

	//法線ベクトルの取得
	float4 normal	= g_normalTexture.Sample(g_sampler, uv) * 2.f - 1.f;	//テクスチャの値を(0~1)から(-1~1)に
	normal.w		= 0;													//wは使わない					
	normal			= normalize(normal);									//正規化

	//テクスチャの有無を確認して描画ピクセル上のオブジェクトの色を設定する
	float4 id;																//オブジェクトの色
	if (g_isTexture == true) id = g_texture.Sample(g_sampler, uv);			//テクスチャを使用していればテクスチャ色
	else					 id = g_diffuseColor;							//使用していなければマテリアル色

	//拡散反射光の取得
	const float4 SHADE		= saturate(dot(LIGHT_DIRECTION, normalize(normal)));						//明るさ
	const float4 DIFFUSE	= SHADE * id;																//拡散反射光

	//鏡面反射光の取得
	float4 specular = float4(0, 0, 0, 0);																//鏡面反射光
	if (g_specular.a != 0)																				//スペキュラー情報の有ればスペキュラー情報を設定する
	{
		const float4 REFLECT	= reflect(LIGHT_DIRECTION, normal);										//正反射ベクトル
		specular				= pow(saturate(dot(REFLECT, inData.eye)), g_shininess) * g_specular;	//ハイライトを求める
	}

	//出力色の計算
	const float DARKEN	= 0.7f;											//暗い色に調整するための値
	float4		color	= (g_ambient + DIFFUSE + specular) * DARKEN;	//出力色
	color.a				= 1.f;											//アルファ値（透明度）は1

	return color;
}