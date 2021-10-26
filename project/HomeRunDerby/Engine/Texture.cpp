#include "Texture.h"

#include "Direct3D.h"
#include "Global.h"

//初期化
Texture::Texture():pSampler_(nullptr), pSRV_(nullptr),imgWidth_(0), imgHeight_(0)
{
}

Texture::~Texture()
{
	Release();
}

//画像ファイルのロード
HRESULT Texture::Load(const std::string& fileName)
{
	IWICFormatConverter* pFormatConverter = nullptr;	//コンバーター

	//コンバーターの初期化
	if (FAILED(InitCOM(fileName, pFormatConverter)))
	{
		MessageBox(nullptr, "2Dテキスチャーの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//画像サイズを調べる
	pFormatConverter->GetSize(&imgWidth_, &imgHeight_);

	//テクスチャを作成
	ID3D11Texture2D*	pTexture;
	CreateTexture(pTexture);

	//テクスチャをコンテキストに渡す
	ID3D11DeviceContext*	 pContext = Direct3D::GetContext();
	D3D11_MAPPED_SUBRESOURCE hMappedres;
	pContext->Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &hMappedres);
	pFormatConverter->CopyPixels(nullptr, imgWidth_ * 4, imgWidth_ * imgHeight_ * 4, (BYTE*)hMappedres.pData);
	pContext->Unmap(pTexture, 0);

	//pTextureのデータから必要なものを抜き出して初期化する
	InitializeTexture2D(pTexture);

	//一時的に取得したデータの解放
	SAFE_RELEASE(pTexture);

	return S_OK;
}

//テクスチャの初期化
HRESULT Texture::InitializeTexture2D(ID3D11Texture2D * pTexture)
{
	//サンプラー作成
	if (FAILED(CreateSampler())) return E_FAIL;

	//シェーダーリソースビュー作成
	if (FAILED(CreateShaderResourceView(pTexture))) return E_FAIL;

	return S_OK;
}

//サンプラーの取得
ID3D11SamplerState* Texture::GetSampler() const
{
	return pSampler_;
}

//シェーダーリソースビューの取得
ID3D11ShaderResourceView* Texture::GetSRV() const
{
	return pSRV_;
}

//幅の取得
UINT Texture::GetWidth() const
{
	return imgWidth_; 
}

//高さの取得
UINT Texture::GetHeight() const
{
	return imgHeight_; 
}

//解放
void Texture::Release()
{
	SAFE_RELEASE(pSRV_);
	SAFE_RELEASE(pSampler_);
}

//ファイルを開く
HRESULT Texture::InitCOM(const std::string& fileName, IWICFormatConverter*& pFormatConverter)
{
	//ファイル名をワイド文字へ
	wchar_t wtext[FILENAME_MAX];
	size_t	ret;
	mbstowcs_s(&ret, wtext, fileName.c_str(), fileName.length());

	//COMの初期化
	if (FAILED(CoInitialize(nullptr)))
	{
		MessageBox(nullptr, "COMの初期化に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//COMのインスタンス
	IWICImagingFactory*		pFactory = nullptr;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, reinterpret_cast<void **>(&pFactory))))
	{
		MessageBox(nullptr, "COMインスタンスの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//画像ファイルの作成
	IWICBitmapDecoder*		pDecoder = nullptr;
	if (FAILED(pFactory->CreateDecoderFromFilename(wtext, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder)))
	{
		MessageBox(nullptr, "画像ファイルの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//コンバーターの作成
	if (FAILED(pFactory->CreateFormatConverter(&pFormatConverter)))
	{
		MessageBox(nullptr, "コンバーターの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//コンバーターの初期化
	IWICBitmapFrameDecode*	pFrame = nullptr;
	pDecoder->GetFrame(0, &pFrame);
	pFormatConverter->Initialize(pFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, NULL, 1.0f, WICBitmapPaletteTypeMedianCut);

	//COMの終了
	CoUninitialize();

	return S_OK;
}

//テクスチャの作成
HRESULT Texture::CreateTexture(ID3D11Texture2D *& pTexture)
{
	D3D11_TEXTURE2D_DESC texdec;
	texdec.Width				= imgWidth_;					//テクスチャの幅（テクセル単位）
	texdec.Height				= imgHeight_;					//テクスチャの高さ
	texdec.MipLevels			= 1;							//テクスチャ内のミップマップレベルの最大数
	texdec.ArraySize			= 1;							//テクスチャ配列内のテクスチャの数
	texdec.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;	//テクスチャフォーマット
	texdec.SampleDesc.Count		= 1;							//ピクセルあたりのマルチサンプルの数
	texdec.SampleDesc.Quality	= 0;							//画質レベル
	texdec.Usage				= D3D11_USAGE_DYNAMIC;			//テクスチャの読み書き方法を識別する値
	texdec.BindFlags			= D3D11_BIND_SHADER_RESOURCE;	//パイプラインステージにバインドするためのフラグ
	texdec.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		//許可されるCPUアクセスのタイプを指定するフラグ
	texdec.MiscFlags			= 0;							//他のあまり一般的ではないリソースオプションを識別するフラグ

	//2Dテキスチャーの作成
	if (FAILED(Direct3D::GetDevice()->CreateTexture2D(&texdec, nullptr, &pTexture)))
	{
		MessageBox(nullptr, "2Dテキスチャーの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//サンプラーの作成
HRESULT Texture::CreateSampler()
{
	D3D11_SAMPLER_DESC  SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter	 = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//テクスチャをサンプリングするときに使用するフィルタリング方法
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//0から1の範囲外のauテクスチャ座標を解決するために使用するメソッド
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//0から1の範囲外のvテクスチャ座標を解決するために使用するメソッド
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//0から1の範囲外のwテクスチャ座標を解決するために使用するメソッド

	if (FAILED(Direct3D::GetDevice()->CreateSamplerState(&SamDesc, &pSampler_)))
	{
		MessageBox(nullptr, "サンプラーの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//シェーダーリソースビューの作成
HRESULT Texture::CreateShaderResourceView(ID3D11Texture2D * pTexture)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format				 = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv.ViewDimension		 = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels	 = 1;

	if (FAILED(Direct3D::GetDevice()->CreateShaderResourceView(pTexture, &srv, &pSRV_)))
	{
		MessageBox(nullptr, "シェーダーリソースビューの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
