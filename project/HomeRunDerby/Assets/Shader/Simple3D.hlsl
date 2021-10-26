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
	//ピクセルシェーダーへ渡す情報を設定
	VS_OUT outData;														//出力データ
	outData.position = mul(position, g_matVertexConversion);			//頂点の座標を頂点変換行列で変換する
	outData.normal	 = mul(normal, g_matNormalConversion);				//頂点の法線を法線変換用行列で変換する
	outData.eye		 = normalize(g_camPos - mul(position, g_matWorld));	//頂点から視点へのベクトル
	outData.uv		 = uv;												//テクスチャの貼り付けに使用する座標

	return 	outData;
}

//ピクセルシェーダ
float4 PS(VS_OUT inData) : SV_Target
{
	//ライト
	const float4 LIGHT			  = float4(-0.3f, -1.f, -0.5f, 0);				//ライト
	const float4 LIGHT_DIRECTION  = normalize(LIGHT);							//ライトの向きベクトル正規化

	//テクスチャの有無を確認して描画ピクセル上のオブジェクトの色を設定する
	float4 id;																	//オブジェクトの色
	if (g_isTexture == true) id = g_texture.Sample(g_sampler, inData.uv);		//テクスチャを使用していればテクスチャ色
	else					 id = g_diffuseColor;								//使用していなければマテリアル色

	//拡散反射光の設定
	const float4 NORMAL_DIRECTION	= normalize(inData.normal);										//法線の向きベクトル正規化
	const float4 SHADE				= saturate(dot(-LIGHT_DIRECTION, NORMAL_DIRECTION));			//明るさ
	const float4 DIFFUSE			= SHADE * id;													//拡散反射光

	//鏡面反射光の設定
	float4 specular = float4(0, 0, 0, 0);															//鏡面反射光
	if (g_specular.a != 0)																			//スペキュラー情報の有ればスペキュラー情報を設定する
	{
		const float4 REFLECT = reflect(LIGHT_DIRECTION, NORMAL_DIRECTION);							//正反射ベクトル
		specular			 = pow(saturate(dot(REFLECT, inData.eye)), g_shininess) * g_specular;	//ハイライトを求める
	}

	float4 color = g_ambient + DIFFUSE + specular;								//出力色 = 環境光 + 拡散反射光 + 鏡面反射光

	return color;
}