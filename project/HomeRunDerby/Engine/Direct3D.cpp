#include "Direct3D.h"

#include <d3dcompiler.h>

#include "Texture.h"
#include "Camera.h"
#include "Global.h"


namespace Direct3D
{
	using DirectX::XMVECTOR;

	namespace
	{
		//シェーダごとの設定項目
		struct ShaderBundle
		{
			ID3D11VertexShader*		pVertexShader		= nullptr;			//頂点シェーダー
			ID3D11PixelShader*		pPixelShader		= nullptr;			//ピクセルシェーダー
			ID3D11InputLayout*		pVertexLayout		= nullptr;			//頂点インプットレイアウト
			ID3D11RasterizerState*	pRasterizerState	= nullptr;			//ラスタライザー
		};

		////////////////////
		// 変数
		ID3D11Device*				pDevice_			= nullptr;			//デバイス
		ID3D11DeviceContext*		pContext_			= nullptr;			//デバイスコンテキスト
		IDXGISwapChain*				pSwapChain_			= nullptr;			//スワップチェイン
		ID3D11RenderTargetView*		pRenderTargetView_	= nullptr;			//レンダーターゲットビュー
		ID3D11Texture2D*			pDepthStencil_		= nullptr;			//深度ステンシル
		ID3D11DepthStencilView*		pDepthStencilView_	= nullptr;			//深度ステンシルビュー
		ID3D11BlendState*			pBlendState_		= nullptr;			//ブレンドステート
		ShaderBundle				shaderBundle_[(int)SHADER_TYPE::MAX];	//シェーダごとの設定項目をまとめたもの
		SHADER_TYPE					currentShaderType_;						//現在のシェーダータイプ
	}

	////////////////////
	// 関数

	//初期化
	HRESULT Initialize(HWND &hWnd)
	{
		//諸設定,デバイス、コンテキスト、スワップチェインの作成
		if (FAILED(InitAdvance(hWnd))) return E_FAIL;

		//ビューの準備
		if (FAILED(InitView())) return E_FAIL;

		//シェーダー準備
		if (FAILED(InitShader())) return E_FAIL;

		//カメラ準備
		Camera::Initialize();

		return S_OK;
	}

	//諸設定,デバイス、コンテキスト、スワップチェインの作成
	HRESULT InitAdvance(HWND &hWnd)
	{
		//スワップチェインの設定
		DXGI_SWAP_CHAIN_DESC scDesc;													//スワップチェインの設定項目をまとめた構造体
		ZeroMemory(&scDesc, sizeof(scDesc));
		scDesc.BufferDesc.Width						= Global::WINDOW_WIDTH;				//画面幅
		scDesc.BufferDesc.Height					= Global::WINDOW_HEIGHT;			//画面高さ
		scDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;		//何色使えるか
		scDesc.BufferDesc.RefreshRate.Denominator	= 1;								//フレームを指定　分母
		scDesc.BufferDesc.RefreshRate.Numerator		= Global::GAME_FPS;					//フレームを指定　分子
		scDesc.Windowed								= TRUE;								//ウィンドウモードかフルスクリーンか
		scDesc.OutputWindow							= hWnd;								//ウィンドウハンドル
		scDesc.BufferCount							= 1;								//バックバッファの枚数
		scDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;	//バックバッファの使い道＝画面に描画するために
		scDesc.SampleDesc.Count						= 1;								//アンチエイリアスの設定
		scDesc.SampleDesc.Quality					= 0;								//画質レベル（アンチエイリアス無しなら0）

		//上記設定をもとにデバイス、コンテキスト、スワップチェインを作成
		D3D_FEATURE_LEVEL level;
		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,						//どのビデオアダプタを使用するか？既定ならばnullptrで
			D3D_DRIVER_TYPE_HARDWARE,		//ドライバのタイプを渡す。ふつうはHARDWARE
			nullptr,						//上記をD3D_DRIVER_TYPE_SOFTWAREに設定しないかぎりnullptr
			0,								//何らかのフラグを指定する。（デバッグ時はD3D11_CREATE_DEVICE_DEBUG？）
			nullptr,						//デバイス、コンテキストのレベルを設定。nullptrにしとけばOK
			0,								//上の引数でレベルを何個指定したか
			D3D11_SDK_VERSION,				//SDKのバージョン。必ずこの値
			&scDesc,						//スワップチェインの設定項目をまとめた構造体
			&pSwapChain_,					//完成したSwapChainのアドレスが返ってくる
			&pDevice_,						//完成したDeviceアドレスが返ってくる
			&level,							//完成したDevice、Contextのレベルが返ってくる
			&pContext_ 						//完成したContextのアドレスが返ってくる
		);

		if (FAILED(hr)){
			MessageBox(nullptr, "デバイス、コンテキスト、スワップチェインの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//ビューの準備
	HRESULT InitView()
	{
		{//レンダーターゲットビュー作成

			//スワップチェーンからバックバッファを取得
			ID3D11Texture2D* pBackBuffer = nullptr;

			//バックバッファの取得エラーチェック
			if (FAILED(pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
			{
				MessageBox(nullptr, "バックバッファの取得に失敗しました", "エラー", MB_OK);
				return E_FAIL;
			}

			//レンダーターゲットビューを作成
			if (FAILED(pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView_)))
			{
				MessageBox(nullptr, "レンダーターゲットビューの作成に失敗しました", "エラー", MB_OK);
				return E_FAIL;
			}

			//一時的にバックバッファを取得しただけなので解放
			SAFE_RELEASE(pBackBuffer);
		}

		{//ビューポート（描画範囲）設定

			//レンダリング結果を表示する範囲
			D3D11_VIEWPORT vp;
			vp.Width	= (float)Global::WINDOW_WIDTH;	//幅
			vp.Height	= (float)Global::WINDOW_HEIGHT;	//高さ
			vp.MinDepth = 0.0f;							//手前
			vp.MaxDepth = 1.0f;							//奥
			vp.TopLeftX = 0;							//左
			vp.TopLeftY = 0;							//上

			//データを画面に描画するための一通りの設定（パイプライン）
			pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);			// データの入力種類を指定
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);			// 描画先を設定
			pContext_->RSSetViewports(1, &vp);
			
			//深度ステンシルビューの設定
			D3D11_TEXTURE2D_DESC descDepth;
			descDepth.Width					= Global::WINDOW_WIDTH;		//テクスチャの幅（テクセル単位）
			descDepth.Height				= Global::WINDOW_HEIGHT;	//テクスチャの高さ
			descDepth.MipLevels				= 1;						//テクスチャ内のミップマップレベルの最大数
			descDepth.ArraySize				= 1;						//テクスチャ配列内のテクスチャの数
			descDepth.Format				= DXGI_FORMAT_D32_FLOAT;	//テクスチャフォーマット
			descDepth.SampleDesc.Count		= 1;						//ピクセルあたりのマルチサンプルの数
			descDepth.SampleDesc.Quality	= 0;						//画質レベル
			descDepth.Usage					= D3D11_USAGE_DEFAULT;		//テクスチャの読み書き方法を識別する値
			descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;	//パイプラインステージにバインドするためのフラグ
			descDepth.CPUAccessFlags		= 0;						//許可されるCPUアクセスのタイプを指定するフラグ
			descDepth.MiscFlags				= 0;						//他のあまり一般的ではないリソースオプションを識別するフラグ
		
			//深度ステンシルビュー用のテクスチャの作成
			if (FAILED(pDevice_->CreateTexture2D(&descDepth, NULL, &pDepthStencil_)))
			{
				MessageBox(nullptr, "深度ステンシルビュー用のテクスチャの作成に失敗しました", "エラー", MB_OK);
				return E_FAIL;
			}

			//深度ステンシルビューの作成
			if (FAILED(pDevice_->CreateDepthStencilView(pDepthStencil_, NULL, &pDepthStencilView_)))
			{
				MessageBox(nullptr, "深度ステンシルビューの作成に失敗しました", "エラー", MB_OK);
				return E_FAIL;
			}

			//描画先を設定
			pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);            
		}

		return S_OK;
	}

	//シェーダー準備
	HRESULT InitShader()
	{
		//3D描画用
		if (FAILED(InitShader3D()))			return E_FAIL;

		//2D描画用
		if (FAILED(InitShader2D()))			return E_FAIL;

		//シェードを使用しない3D描画用のシェーダ準備
		if (FAILED(InitNoShadeShader3D()))	return E_FAIL;

		//空背景描画用のシェーダ準備
		if (FAILED(InitShaderSky()))		return E_FAIL;

		//地面描画用のシェーダ準備
		if (FAILED(InitShaderGround()))		return E_FAIL;

		//ブレンドステートの準備
		D3D11_BLEND_DESC BlendDesc;
		ZeroMemory(&BlendDesc, sizeof(BlendDesc));
		BlendDesc.AlphaToCoverageEnable					= FALSE;						//ピクセルをレンダーターゲットに設定するときに、アルファからカバレッジをマルチサンプリング手法として使用するか
		BlendDesc.IndependentBlendEnable				= FALSE;						//同時レンダーターゲットで独立したブレンディングを有効にする
		BlendDesc.RenderTarget[0].BlendEnable			= TRUE;							//ブレンディングを使用するか
		BlendDesc.RenderTarget[0].SrcBlend				= D3D11_BLEND_ONE;				//ピクセルシェーダーが出力するRGB値に対して実行する操作を指定
		BlendDesc.RenderTarget[0].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;	//レンダーターゲットの現在のRGB値に対して実行する操作を指定
		BlendDesc.RenderTarget[0].BlendOp				= D3D11_BLEND_OP_ADD;			//結合する方法を定義しSrcBlendとDestBlend動作を制御
		BlendDesc.RenderTarget[0].SrcBlendAlpha			= D3D11_BLEND_ONE;				//ピクセルシェーダーが出力するアルファ値に対して実行する操作を指定
		BlendDesc.RenderTarget[0].DestBlendAlpha		= D3D11_BLEND_ZERO;				//レンダーターゲットの現在のアルファ値に対して実行する操作
		BlendDesc.RenderTarget[0].BlendOpAlpha			= D3D11_BLEND_OP_ADD;			//どのように組み合わせるかを定義しSrcBlendAlphaとDestBlendAlpha動作を制御
		BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;	//書き込みマスク
	
		//ブレンドステートの作成
		if (FAILED(pDevice_->CreateBlendState(&BlendDesc, &pBlendState_)))
		{
			return E_FAIL;
		}

		//出力マージステージのブレンド状態を設定
		const float BLEND_FACTOR[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };	//RGBAコンポーネントごとのブレンドファクターの配列
		pContext_->OMSetBlendState(pBlendState_, BLEND_FACTOR, 0xffffffff);

		return S_OK;
	}

	//使用するシェーダーに設定
	void SetShaderBundle(SHADER_TYPE type)
	{
		currentShaderType_ = type;													//現在のシェーダを変更
		pContext_->VSSetShader(shaderBundle_[(int)type].pVertexShader, NULL, 0);	//頂点シェーダー
		pContext_->PSSetShader(shaderBundle_[(int)type].pPixelShader, NULL, 0);		//ピクセルシェーダー
		pContext_->IASetInputLayout(shaderBundle_[(int)type].pVertexLayout);		//頂点インプットレイアウト
		pContext_->RSSetState(shaderBundle_[(int)type].pRasterizerState);			//ラスタライザー
	}

	//3Dシェーダ準備
	HRESULT InitShader3D()
	{
		// 頂点シェーダの指定（コンパイル）
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);

		// 頂点シェーダの作成
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pVertexShader)))
		{
			MessageBox(nullptr, "Simple3D.hlslの頂点シェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		//頂点インプットレイアウトの設定
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                   , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV座標 
					{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 },	//法線
		};

		//頂点インプットレイアウトの作成
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pVertexLayout)))
		{
			MessageBox(nullptr, "Simple3D.hlslの頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// ピクセルシェーダ指定（コンパイル）
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Simple3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);

		// ピクセルシェーダの作成
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pPixelShader)))
		{
			MessageBox(nullptr, "Simple3D.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//ラスタライザの設定
		D3D11_RASTERIZER_DESC rdc	 = {};
		rdc.CullMode				 = D3D11_CULL_BACK;		//描画設定　ポリゴン表裏
		rdc.FillMode				 = D3D11_FILL_SOLID;	//塗りつぶし
		rdc.FrontCounterClockwise	 = FALSE;				//反時計回りは裏

		//ラスタライザ作成
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_3D].pRasterizerState)))
		{
			MessageBox(nullptr, "Simple3D.hlslのラスタライザの作成に失敗しました","エラー", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//2Dシェーダ準備
	HRESULT InitShader2D()
	{
		// 頂点シェーダの指定（コンパイル）
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);
		
		// 頂点シェーダの作成
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pVertexShader)))
		{
			MessageBox(nullptr, "頂点シェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		//頂点インプットレイアウトの設定
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0               , D3D11_INPUT_PER_VERTEX_DATA, 0},	//位置
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR), D3D11_INPUT_PER_VERTEX_DATA, 0},	//UV座標 
		};

		//頂点インプットレイアウトの作成
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pVertexLayout)))
		{
			MessageBox(nullptr, "頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// ピクセルシェーダの作成（コンパイル）
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Simple2D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pPixelShader)))
		{
			MessageBox(nullptr, "ピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//ラスタライザ作成
		D3D11_RASTERIZER_DESC rdc	 = {};
		rdc.CullMode				 = D3D11_CULL_BACK;		//描画設定　ポリゴン表裏
		rdc.FillMode				 = D3D11_FILL_SOLID;	//塗りつぶし
		rdc.FrontCounterClockwise	 = FALSE;				//反時計回りは裏
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::DEFAULT_2D].pRasterizerState)))
		{
			MessageBox(nullptr, "ラスタライザの作成に失敗しました",
				"エラー", MB_OK);
			return E_FAIL;
		}
		return S_OK;
	}

	//シェードを使用しない3D描画用のシェーダ準備
	HRESULT InitNoShadeShader3D()
	{
		// 頂点シェーダの指定（コンパイル）
		ID3DBlob *pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/NoShade3D.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		assert(pCompileVS != nullptr);

		// 頂点シェーダの作成
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pVertexShader)))
		{
			MessageBox(nullptr, "NoShade3D.hlslの頂点シェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		//頂点インプットレイアウトの設定
		D3D11_INPUT_ELEMENT_DESC layout[] = {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                   , D3D11_INPUT_PER_VERTEX_DATA, 0},		//位置
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)    , D3D11_INPUT_PER_VERTEX_DATA, 0},		//UV座標 
					{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2, D3D11_INPUT_PER_VERTEX_DATA, 0},		//法線
		};

		//頂点インプットレイアウトの作成
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pVertexLayout)))
		{
			MessageBox(nullptr, "NoShade3D.hlslの頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);

		// ピクセルシェーダ指定（コンパイル）
		ID3DBlob *pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/NoShade3D.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);
		assert(pCompilePS != nullptr);

		// ピクセルシェーダの作成
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pPixelShader)))
		{
			MessageBox(nullptr, "NoShade3D.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//ラスタライザの設定
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//描画設定　ポリゴン表裏
		rdc.FillMode				= D3D11_FILL_SOLID;		//塗りつぶし
		rdc.FrontCounterClockwise	= FALSE;				//反時計回りは裏

		//ラスタライザ作成
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::NO_SHADE_3D].pRasterizerState)))
		{
			MessageBox(nullptr, "NoShade3D.hlslのラスタライザの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT Direct3D::InitShaderSky()
	{
		// 頂点シェーダの作成（コンパイル）
		ID3DBlob* pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Sky.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pVertexShader)))
		{
			MessageBox(nullptr, "Sky.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		//頂点インプットレイアウト
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)     , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV座標
		};

		//頂点インプットレイアウトの作成
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pVertexLayout)))
		{
			MessageBox(nullptr, "Sky.hlslの頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);


		// ピクセルシェーダの作成（コンパイル）
		ID3DBlob* pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Sky.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);

		// ピクセルシェーダの作成
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pPixelShader)))
		{
			MessageBox(nullptr, "Sky.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//ラスタライザの設定
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//描画設定　ポリゴン表裏
		rdc.FillMode				= D3D11_FILL_SOLID;		//塗りつぶし
		rdc.FrontCounterClockwise	= FALSE;				//反時計回りは裏

		//ラスタライザ作成
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::SKY_2D].pRasterizerState)))
		{
			MessageBox(nullptr, "Sky.hlslのラスタライザの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//地面描画用のシェーダ準備
	HRESULT InitShaderGround()
	{
		// 頂点シェーダの作成（コンパイル）
		ID3DBlob* pCompileVS = nullptr;
		D3DCompileFromFile(L"Shader/Ground.hlsl", nullptr, nullptr, "VS", "vs_5_0", NULL, 0, &pCompileVS, NULL);
		if (FAILED(pDevice_->CreateVertexShader(pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::GROUND].pVertexShader)))
		{
			MessageBox(nullptr, "Ground.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		//頂点インプットレイアウト
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0                    , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//位置
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, sizeof(XMVECTOR)     , D3D11_INPUT_PER_VERTEX_DATA, 0 },	//UV座標
			{ "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 2 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	//法線
			{ "TANGENT" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(XMVECTOR) * 3 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },	//接線
		};

		//頂点インプットレイアウトの作成
		if (FAILED(pDevice_->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), pCompileVS->GetBufferPointer(), pCompileVS->GetBufferSize(), &shaderBundle_[(int)SHADER_TYPE::GROUND].pVertexLayout)))
		{
			MessageBox(nullptr, "Ground.hlslの頂点インプットレイアウトの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompileVS);


		// ピクセルシェーダの作成（コンパイル）
		ID3DBlob* pCompilePS = nullptr;
		D3DCompileFromFile(L"Shader/Ground.hlsl", nullptr, nullptr, "PS", "ps_5_0", NULL, 0, &pCompilePS, NULL);

		// ピクセルシェーダの作成
		if (FAILED(pDevice_->CreatePixelShader(pCompilePS->GetBufferPointer(), pCompilePS->GetBufferSize(), NULL, &shaderBundle_[(int)SHADER_TYPE::GROUND].pPixelShader)))
		{
			MessageBox(nullptr, "Ground.hlslのピクセルシェーダの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}
		SAFE_RELEASE(pCompilePS);

		//ラスタライザの設定
		D3D11_RASTERIZER_DESC rdc	= {};
		rdc.CullMode				= D3D11_CULL_BACK;		//描画設定　ポリゴン表裏
		rdc.FillMode				= D3D11_FILL_SOLID;		//塗りつぶし
		rdc.FrontCounterClockwise	= FALSE;				//反時計回りは裏

		//ラスタライザ作成
		if (FAILED(pDevice_->CreateRasterizerState(&rdc, &shaderBundle_[(int)SHADER_TYPE::GROUND].pRasterizerState)))
		{
			MessageBox(nullptr, "Ground.hlslのラスタライザの作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		return S_OK;
	}

	//描画開始
	void BeginDraw()
	{
		//背景の色
		float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };//R,G,B,A

		//画面をクリア
		pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);

		//カメラ
		Camera::Update();

		//深度バッファクリア
		pContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//描画終了
	void EndDraw()
	{
		//スワップ（バックバッファを表に表示する）
		pSwapChain_->Present(0, 0);
	}

	//解放処理
	void Release()
	{
		for (int i = 0; i < (int)SHADER_TYPE::MAX; i++) {
			SAFE_RELEASE(shaderBundle_[i].pRasterizerState);
			SAFE_RELEASE(shaderBundle_[i].pVertexLayout);
			SAFE_RELEASE(shaderBundle_[i].pPixelShader);
			SAFE_RELEASE(shaderBundle_[i].pVertexShader);
		}

		SAFE_RELEASE(pDepthStencilView_);
		SAFE_RELEASE(pDepthStencil_);
		SAFE_RELEASE(pRenderTargetView_);
		SAFE_RELEASE(pSwapChain_);
		SAFE_RELEASE(pContext_);
		SAFE_RELEASE(pDevice_);
	}

	//デバイスを取得
	ID3D11Device * GetDevice()
	{
		 return pDevice_; 
	}

	//デバイスコンテキストを取得
	ID3D11DeviceContext * GetContext()
	{
		return pContext_;
	}
	SHADER_TYPE GetCurrentShaderType()
	{
		return currentShaderType_;
	}
}