#pragma once
#include <dInput.h>
#include <DirectXMath.h>

#include "XInput.h"				

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")	
#pragma comment(lib, "Xinput.lib")	

//入力管理
namespace Input
{
	using DirectX::XMVECTOR;

	//初期化
	//引数：hWnd ウィンドウハンドル
	//戻値：処理の成否
	HRESULT Initialize(const HWND&);

	//更新
	//引数：なし
	//戻値：なし
	void Update();

	//キーが押されているかを調べる
	//引数：keyCode 判定するキーの番号
	//戻値：真偽
	bool IsKey(int keyCode);

	//キーが今押されたかを調べる
	//引数：keyCode 判定するキーの番号
	//戻値：真偽
	bool IsKeyDown(int keyCode);

	//キーが今離されたかを調べる
	//引数：keyCode 判定するキーの番号
	//戻値：真偽
	bool IsKeyUp(int keyCode);

	//マウスが押されているかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsMouseButton(int buttonCode);

	//マウスが今押されたかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsMouseButtonDown(int buttonCode);

	//マウスが今離されたかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsMouseButtonUp(int buttonCode);

	//マウスの位置をセットする
	//引数：x マウスのx座標
	//引数：y マウスのy座標
	//戻値：なし
	void SetMousePosition(int x, int y);

	//マウスの座標を取得
	//戻値：マウスの座標
	XMVECTOR GetMousePosition();

	//マウスの移動量を取得
	//戻値：マウスの移動量
	XMVECTOR GetMouseMove();

	//ゲームパッドが押されているかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsPadButton(int buttonCode);

	//ゲームパッドが今押されたかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsPadButtonDown(int buttonCode);

	//ゲームパッドが今離されたかを調べる
	//引数：buttonCode 判定するキーの番号
	//戻値：真偽
	bool IsPadButtonUp(int buttonCode);

	//コントローラスティックの傾きの比率を取得
	//引数：analog　スティックの傾き
	//引数：max　スティックの傾きの最大値
	//引数：deadZone　スティックの遊び
	//戻値：スティックの傾きの比率
	float GetAnalogValue(int analog, int max, int deadZone);

	//左コントローラスティックの傾きを取得
	//戻値：左コントローラスティックの傾き
	XMVECTOR GetPadStickL();

	//右コントローラスティックの傾きを取得
	//戻値：右コントローラスティックの傾き
	XMVECTOR GetPadStickR();

	//左トリガーボタンの傾きを取得
	//戻値：左コントローラスティックの傾き
	float	GetPadTriggerL();
	
	//右トリガーボタンの傾きを取得
	//戻値：右コントローラスティックの傾き
	float	GetPadTriggerR();
	
	//振動エフェクト
	//引数：left　左の振動の大きさ 0-1
	//引数：right 右の振動の大きさ 0-1
	void SetPadVibration(float left, float right);

	//解放
	void Release();
};