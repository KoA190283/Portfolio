#include <stdlib.h>
#include <Windows.h>

#include "../ExternalDataManager.h"
#include "Direct3D.h"
#include "Global.h"
#include "Image.h"
#include "Input.h"
#include "Model.h"
#include "RootJob.h"

#pragma comment(lib, "winmm.lib")

//定数宣言
const char* WIN_CLASS_NAME	= "HomeRun Derby";		//ウィンドウクラス名

//プロトタイプ宣言
HWND InitWindow(HINSTANCE hInstance, int nCmdShow);											//ウィンドウの準備
bool InitEngine(HWND hWnd);																	//エンジン部分の初期化
void MessLoop(HWND hWnd);																	//メッセージループ
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);				//ウィンドウプロシージャ

//クラスポインタ
static RootJob* pRootJob = nullptr;															//ゲームオブジェクトのルート

//エントリーポイント
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	srand((unsigned)time(NULL));					//乱数設定
	SetCurrentDirectory("Assets");					//カレントディレクトリの変更

	//ウィンドウ関連の準備
	HWND hWnd = InitWindow(hInstance, nCmdShow);	//ウィンドウの準備
	ShowWindow(hWnd, nCmdShow);						//ウィンドウを表示

	//エンジン部分の初期化
	if (!InitEngine(hWnd)) return 0;

	//メッセージループ
	MessLoop(hWnd);

	//動的確保した変数の解放・削除
	pRootJob->ReleaseSub();
	SAFE_DELETE(pRootJob);
	Input::Release();
	Direct3D::Release();

	return 0;
}

//ウィンドウの準備
HWND InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//ウィンドウクラス（設計図）を作成
	WNDCLASSEX wc;
	wc.cbSize		 = sizeof(WNDCLASSEX);					//この構造体のサイズ
	wc.hInstance	 = hInstance;							//インスタンスハンドル
	wc.lpszClassName = WIN_CLASS_NAME;						//ウィンドウクラス名
	wc.lpfnWndProc	 = WndProc;								//ウィンドウプロシージャ
	wc.style		 = CS_VREDRAW | CS_HREDRAW;				//スタイル（デフォルト）
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);		//アイコン
	wc.hIconSm		 = LoadIcon(NULL, IDI_WINLOGO);			//小さいアイコン
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);			//マウスカーソル
	wc.lpszMenuName	 = NULL;								//メニュー（なし）
	wc.cbClsExtra	 = 0;									//システムが内部で管理するWNDCLASS構造体に追加するメモリのサイズ
	wc.cbWndExtra	 = 0;									//ウインドウを表す内部データ構造体に追加するメモリ
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //背景（黒）
	RegisterClassEx(&wc);									//クラスを登録

	//ウィンドウサイズの計算
	RECT winRect = { 0, 0, Global::WINDOW_WIDTH, Global::WINDOW_HEIGHT };	//ウィンドウのサイズ情報
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);					//調整
	int	 winW	 = winRect.right - winRect.left;							//ウィンドウ幅
	int  winH	 = winRect.bottom - winRect.top;							//ウィンドウ高さ

	//ウィンドウを作成
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,		 //ウィンドウクラス名
		WIN_CLASS_NAME,		 //タイトルバーに表示する内容
		WS_OVERLAPPEDWINDOW, //スタイル（普通のウィンドウ）
		CW_USEDEFAULT,       //表示位置左（おまかせ）
		CW_USEDEFAULT,       //表示位置上（おまかせ）
		winW,				 //ウィンドウ高さ
		winH,				 //ウィンドウ幅
		NULL,                //親ウインドウ（なし）
		NULL,                //メニュー（なし）
		hInstance,           //インスタンス
		NULL                 //パラメータ（なし）
	);

	return hWnd;
}

//エンジン部分の初期化
bool InitEngine(HWND hWnd)
{
	//Direct3D初期化
	if (FAILED(Direct3D::Initialize(hWnd)))
	{
		Direct3D::Release();
		return false;
	}

	//Input初期化
	if (FAILED(Input::Initialize(hWnd)))
	{
		Input::Release();
		Direct3D::Release();
		return false;
	}

	//ルートジョブ初期化
	pRootJob = new RootJob;
	pRootJob->Initialize();

	return true;
}

//メッセージループ（何か起きるのを待つ）
void MessLoop(HWND hWnd)
{
	MSG msg;						//メッセージ
	ZeroMemory(&msg, sizeof(msg));	//初期化

	//終了メッセージが来たら終了
	while (msg.message != WM_QUIT)
	{
		//メッセージあり
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);							//仮想キーコードを実際の文字に変換
			DispatchMessage(&msg);							//ウィンドウプロシージャへメッセージを送出する
		}

		//メッセージなし
		else
		{
			timeBeginPeriod(1);								//取得する時間の精度を上げる

			DWORD		 nowTime		 = timeGetTime();	//現在の時間
			static DWORD startTime		 = nowTime;			//開始時間
			static DWORD lastUpdateTime	 = nowTime;			//最終更新時間
			static DWORD countFps		 = 0;				//更新回数

			//1000ミリ秒（1秒）たった
			const int	MILLISECOND = 1000;					//1/1000秒

			//規定更新速度より早かったらなにもしない
			DWORD updateTime = (nowTime - lastUpdateTime) * Global::GAME_FPS;	//前回の更新からの時間
			if (updateTime <= MILLISECOND) continue;							//規定速度より早ければ更新せずにループの先頭へ

			lastUpdateTime = nowTime;						//最終更新時間の更新
			countFps++;										//更新回数
		
			//情報の更新
			Input::Update();								//入力情報の更新
			pRootJob->UpdateSub();							//GameObjectの更新

			if (pRootJob->IsEndGame()) break;				//終了判定がtrueならメッセージループを抜ける

			//描画処理
			Direct3D::BeginDraw();							//描画情報の初期化
			pRootJob->DrawSub();							//GameObjectの描画
			Direct3D::EndDraw();							//描画

			Sleep(1);										//1 m/sec休む
			timeEndPeriod(1);								//時間取得の精度を戻す
		}
	}

	//アセットデータの解放
	ExternalDataManager::Release();
	Model::Release();
	Image::Release();
}

//ウィンドウプロシージャ（何かあった時によばれる関数）
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//メッセージで分岐
	switch (msg)
	{
	//マウスが動いた
	case WM_MOUSEMOVE:	
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));	//マウスの位置を設定
		return 0;

	//ウィンドウが閉じられた
	case WM_DESTROY:	
		PostQuitMessage(0);											//プログラム終了
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}