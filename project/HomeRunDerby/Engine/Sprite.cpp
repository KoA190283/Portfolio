#include "Sprite.h"

#include "Global.h"
#include "Texture.h"

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

Sprite::Sprite()
{
	pVertexBuffer_		= nullptr;		//頂点バッファ
	pIndexBuffer_		= nullptr;		//インデックスバッファ
	pConstantBuffer_	= nullptr;		//コンスタントバッファ
	pTexture_			= nullptr;		//テクスチャ
	indexNum_			= 0;			//インデックス数
}

Sprite::~Sprite()
{
	Release();
}

//画像データを扱うための準備
HRESULT Sprite::Initialize(const std::string& filename)
{
	//頂点バッファ準備
	if (FAILED(InitVertex())) return E_FAIL;

	//インデックスバッファ準備
	if (FAILED(InitIndex())) return E_FAIL;
	
	//コンスタントバッファ準備
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//テキスチャーのロード
	pTexture_ = new Texture;
	if (FAILED(pTexture_->Load(filename.c_str()))) return E_FAIL;
	
	return S_OK;
}

//画像データを扱うための準備
HRESULT Sprite::Initialize(ID3D11Texture2D*	pTexture)
{
	//頂点バッファ準備
	if (FAILED(InitVertex())) return E_FAIL;

	//インデックスバッファ準備
	if (FAILED(InitIndex())) return E_FAIL;

	//コンスタントバッファ準備
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//テキスチャーのロード
	pTexture_ = new Texture;
	if (FAILED(pTexture_->InitializeTexture2D(pTexture))) return E_FAIL;

	return S_OK;
}

//描画
void Sprite::Draw(Transform & transform) const
{
	ID3D11DeviceContext*	pContext	= Direct3D::GetContext();	//デバイスコンテキストのポインタ
	static float			scroll		= 0.f;						//経過時間

	//現在のシェーダが空描画用のものであればスクロール変数の値を0.001大きくする
	if (Direct3D::GetCurrentShaderType() == Direct3D::SHADER_TYPE::SKY_2D) scroll += 0.001f;

	//コンスタントバッファに渡す値の準備
	CONSTANT_BUFFER cb;
	XMMATRIX		adjustScale	= DirectX::XMMatrixScaling((float)pTexture_->GetWidth(), (float)pTexture_->GetHeight(), 1.f);												//画像サイズ
	XMMATRIX		miniScale	= DirectX::XMMatrixScaling((1.f / Global::WINDOW_WIDTH), (1.f / Global::WINDOW_HEIGHT), 1.f);												//ウィンドウサイズ
	cb.vertexConversionMatrix	= XMMatrixTranspose(adjustScale * transform.GetScaleMatrix() * transform.GetRotateMatrix() * miniScale * transform.GetTranslateMatrix());	//各サイズと描画する状態を乗算
	cb.scroll					= scroll;

	//サンプラー（テクスチャの表示の仕方）を設定
	ID3D11SamplerState*			pSampler = pTexture_->GetSampler();
	pContext->PSSetSamplers(0, 1, &pSampler);

	//シェーダーリソースビュー（テクスチャ）を設定
	ID3D11ShaderResourceView*	pSRV	 = pTexture_->GetSRV();
	pContext->PSSetShaderResources(0, 1, &pSRV);

	//頂点バッファをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//インデックスバッファーをセット
	stride		= sizeof(int);
	offset		= 0;
	pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//コンスタントバッファ
	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);					//頂点シェーダー用
	pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);					//ピクセルシェーダー用

	//コンスタントバッファの書き換え
	D3D11_MAPPED_SUBRESOURCE	data;
	pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);		//GPUからのデータアクセスを止める
	memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));				//データ値を送る
	pContext->Unmap(pConstantBuffer_, 0);										//再開

	//描画												
	pContext->DrawIndexed(indexNum_, 0, 0);
}

UINT Sprite::GetWidth() const
{
	return pTexture_->GetWidth();
}

UINT Sprite::GetHeight() const
{
	return pTexture_->GetHeight();
}

//解放
void Sprite::Release()
{
	SAFE_RELEASE(pTexture_);
	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}

//頂点情報の設定
HRESULT Sprite::InitVertex()
{
	using DirectX::XMVectorSet;

	//頂点情報
	VERTEX vertices[] =
	{
		{ XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) },   // 四角形の頂点（左上）
		{ XMVectorSet( 1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) },   // 四角形の頂点（右上）
		{ XMVectorSet( 1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f) },   // 四角形の頂点（右下）
		{ XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },   // 四角形の頂点（左下）
	};

	//頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth				= sizeof(vertices);			//バッファーのサイズ
	bd_vertex.Usage					= D3D11_USAGE_DEFAULT;		//バッファーの読み取りおよび書き込みが予想される方法
	bd_vertex.BindFlags				= D3D11_BIND_VERTEX_BUFFER;	//バッファーがパイプラインにバインドされる方法
	bd_vertex.CPUAccessFlags		= 0;						//CPUアクセスフラグ
	bd_vertex.MiscFlags				= 0;						//その他のフラグ
	bd_vertex.StructureByteStride	= 0;						//バッファー構造内の各要素のサイズ

	D3D11_SUBRESOURCE_DATA data_vertex;							//初期化用データ
	data_vertex.pSysMem = vertices;								//初期化データへのポインタ

	//頂点バッファの作成
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_)))
	{
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
//頂点情報の設定
HRESULT Sprite::InitIndex()
{
	int index[] = { 0,2,3,  0,1,2 };					//インデックス情報
	indexNum_	= sizeof(index) / sizeof(int);			//インデックス数

	// インデックスバッファを生成する
	D3D11_BUFFER_DESC   bd;
	bd.ByteWidth			= sizeof(index);			//バッファーのサイズ
	bd.Usage				= D3D11_USAGE_DEFAULT;		//バッファーの読み取りおよび書き込みが予想される方法
	bd.BindFlags			= D3D11_BIND_INDEX_BUFFER;	//バッファーがパイプラインにバインドされる方法
	bd.CPUAccessFlags		= 0;						//CPUアクセスフラグ
	bd.MiscFlags			= 0;						//その他のフラグ

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem			= index;				//初期化データへのポインタ
	initData.SysMemPitch		= 0;					//テクスチャの1つの行の先頭から次の行までの距離
	initData.SysMemSlicePitch	= 0;					//ある深度レベルの先頭から次の深度レベルまでの距離
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd, &initData, &pIndexBuffer_)))
	{
		MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//頂点情報の設定
HRESULT Sprite::InitConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth			= sizeof(CONSTANT_BUFFER);		//バッファーのサイズ
	cb.Usage				= D3D11_USAGE_DYNAMIC;			//バッファーの読み取りおよび書き込みが予想される方法
	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;	//バッファーがパイプラインにバインドされる方法
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		//CPUアクセスフラグ
	cb.MiscFlags			= 0;							//その他のフラグ
	cb.StructureByteStride	= 0;							//バッファー構造内の各要素のサイズ

	//コンスタントバッファの作成
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&cb, nullptr, &pConstantBuffer_)))
	{
		MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
