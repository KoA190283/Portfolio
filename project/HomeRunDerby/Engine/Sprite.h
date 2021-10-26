#pragma once
#include <DirectXMath.h>

#include "Direct3D.h"
#include "Texture.h"
#include "Transform.h"

//2Dオブジェクト（画像）クラス
class Sprite
{
private:

	//コンスタントバッファー
	struct CONSTANT_BUFFER
	{
		using XMMATRIX = DirectX::XMMATRIX;

		XMMATRIX vertexConversionMatrix;	//頂点座標変換行列
		FLOAT	 scroll;					//スクロール
	};

	//頂点情報
	struct VERTEX
	{
		using XMVECTOR = DirectX::XMVECTOR;

		XMVECTOR position;			//画面内の頂点座標
		XMVECTOR uv;				//UV座標
	};

	ID3D11Buffer*	pVertexBuffer_;		//頂点バッファ
	ID3D11Buffer*	pIndexBuffer_;		//インデックスバッファ
	ID3D11Buffer*	pConstantBuffer_;	//コンスタントバッファ
	Texture*		pTexture_;			//テクスチャ
	int				indexNum_;			//インデックス数

public:

	Sprite();
	~Sprite();

	//画像データを扱うための準備
	//引数：fileName	表示する画像ファイル
	//戻値：処理の成否
	HRESULT Initialize(const std::string& fileName);

	//画像データを扱うための準備
	//引数：pTexture	テクスチャ
	//戻値：処理の成否
	HRESULT Initialize(ID3D11Texture2D*	pTexture);

	//描画
	//引数：Transform　ワールド座標
	void Draw(Transform& Transform) const;

	//取得関数
	UINT GetWidth() const;	//幅
	UINT GetHeight() const;	//高さ

	//解放
	void Release();

private:

	//頂点情報の設定
	//戻値：処理の成否
	HRESULT InitVertex();

	//頂点情報の設定
	//戻値：処理の成否
	HRESULT InitIndex();

	//頂点情報の設定
	//戻値：処理の成否
	HRESULT InitConstantBuffer();
};