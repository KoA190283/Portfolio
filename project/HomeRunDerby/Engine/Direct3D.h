#pragma once
#include <d3d11.h>
#include <assert.h>

//リンカ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//3D描画を行うために必要なものを管理
namespace Direct3D
{
	//シェーダーの種類
	enum class SHADER_TYPE
	{
		DEFAULT_3D = 0,		//3D描画用
		DEFAULT_2D,			//2D描画用
		NO_SHADE_3D,		//シェードを使用しない3D描画
		SKY_2D,				//背景の空描画用
		GROUND,				//地面描画用
		MAX
	};

	//初期化
	//引数：hWnd ウィンドウハンドル
	//戻値：処理の成否
	HRESULT Initialize(HWND &hWnd);

	//諸設定
	//引数：scDesc 設定用構造体
	//引数：hWnd ウィンドウハンドル
	//戻値：処理の成否
	HRESULT InitAdvance(HWND &hWnd);

	//ビューの準備
	//戻値：処理の成否
	HRESULT InitView();

	//シェーダー準備
	//戻値：処理の成否
	HRESULT InitShader();

	//使用するシェーダーを設定
	//引数：type シェーダーの種類
	void SetShaderBundle(SHADER_TYPE type);

	//3D描画用のシェーダ準備
	//戻値：処理の成否
	HRESULT InitShader3D();

	//2D描画用のシェーダ準備
	//戻値：処理の成否
	HRESULT InitShader2D();

	//シェードを使用しない3D描画用のシェーダ準備
	//戻値：処理の成否
	HRESULT InitNoShadeShader3D();

	//背景の空描画用のシェーダ準備
	//戻値：処理の成否
	HRESULT InitShaderSky();

	//地面描画用のシェーダ準備
	//戻値：処理の成否
	HRESULT InitShaderGround();

	//描画開始
	void BeginDraw();

	//描画終了
	void EndDraw();

	//解放
	void Release();

	////////////////////
	// 取得関数
	ID3D11Device*		 GetDevice();	//デバイスを取得
	ID3D11DeviceContext* GetContext();	//デバイスコンテキストを取得
	SHADER_TYPE GetCurrentShaderType();	//現在設定しているシェーダータイプを取得
};

