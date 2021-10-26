#pragma once
#include <windows.h>

#define SAFE_DELETE(p)			{if ((p)!=nullptr) { delete (p); (p)=nullptr;}}		//ポインタの削除
#define SAFE_DELETE_ARRAY(p)	{if ((p)!=nullptr) { delete[] (p); (p)=nullptr;}}	//ポインタ配列の削除
#define SAFE_RELEASE(p)			{if ((p)!=nullptr) { p->Release(); (p)=nullptr;}}	//ポインタのRelease関数呼び出し後削除

//複数オブジェクトに参照される定数・関数群
namespace Global
{
	//iniファイルから読み込んだもの
	static const int WINDOW_WIDTH	= GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");		//スクリーンの幅
	static const int WINDOW_HEIGHT	= GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");	//スクリーンの高さ
	static const int GAME_FPS		= GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");			//FPS（画面更新速度）
}