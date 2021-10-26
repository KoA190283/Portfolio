#pragma once
#include <d3d11.h>
#include <wincodec.h>

#include "string"

#pragma comment( lib, "WindowsCodecs.lib" )

//テクスチャクラス
class Texture
{
private:

	ID3D11SamplerState*			pSampler_;	//サンプラー
	ID3D11ShaderResourceView*	pSRV_;		//シェーダーリソースビュー

	UINT						imgWidth_;	//画像サイズ　幅
	UINT						imgHeight_;	//画像サイズ　高さ

public:

	Texture();
	~Texture();

	//画像ファイルのロード
	//引数：fileName ファイル名
	//戻値：処理の成否
	HRESULT Load(const std::string& fileName);

	//テクスチャの初期化
	//引数：pTexture			テクスチャデータ
	//戻値：処理の成否
	HRESULT InitializeTexture2D(ID3D11Texture2D * pTexture);

	//解放
	//引数：なし
	//戻値：なし
	void Release();

	//取得関数
	ID3D11SamplerState*			GetSampler() const;	//サンプラー
	ID3D11ShaderResourceView*	GetSRV() const;		//シェーダーリソースビュー
	UINT						GetWidth() const;	//幅
	UINT						GetHeight() const;	//高さ

private:

	//ファイルを開く
	//引数：fileName			ファイル名
	//引数：pFormatConverter	コンバーター
	//戻値：処理の成否
	HRESULT InitCOM(const std::string& fileName, IWICFormatConverter*& pFormatConverter);

	//テクスチャの作成	
	//引数：pTexture	テクスチャ
	//戻値：処理の成否
	HRESULT CreateTexture(ID3D11Texture2D*&	pTexture);

	//サンプラーの作成
	//戻値：処理の成否
	HRESULT CreateSampler();

	//シェーダーリソースビューの作成
	//引数：pTexture			テクスチャデータ
	//戻値：処理の成否
	HRESULT CreateShaderResourceView(ID3D11Texture2D* pTexture);
};