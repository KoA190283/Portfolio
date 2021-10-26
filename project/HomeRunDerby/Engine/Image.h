#pragma once
#include"Sprite.h"
#include"Transform.h"

//画像データの管理
namespace Image
{
	using DirectX::XMVECTOR;

	const int ERROR_CORD	= -1;	//画像データの取得に失敗した場合の番号
	const int DECIMAL_BASE	= 10;	//10進数の底

	//画像データの読み込み
	//引数：fileName　	ファイルのパス
	//戻値：ハンドル番号
	int Load(const std::string& fileName);

	//表示する状態を設定
	//引数１：handle		ハンドル番号
	//引数２：transform		表示する状態
	void SetTransform(int handle, Transform& transform);

	//描画
	//引数：handle			ハンドル番号
	void Draw(int handle);

	//描画
	//引数１：handle　		ハンドル番号
	//引数２：type　		描画に使用するシェーダーの種類
	void Draw(int handle, Direct3D::SHADER_TYPE type);

	//自然数の描画
	//引数１:handle[DECIMAL_BASE]	0~9の数字画像イメージハンドル
	//引数２:drawNumber				描画する数字
	//引数３:lastDigitTransform		表示する状態（position_は最下位桁の描画位置）
	//引数４:intervalWide			数字ごとの横幅
	void DrawNaturalNumber(int handle[DECIMAL_BASE], int drawNumber, const Transform & lastDigitTransform, float intervalWide);

	//ロード済み画像のハンドル番号を取得する
	//引数：fileName　	ファイルのパス
	//戻値：ハンドル番号
	int GetHandle(const std::string& fileName);

	//幅を取得
	//引数：handle		ハンドル番号
	//戻値：幅
	float GetWidth(int handle);

	//高さを取得
	//引数：handle		ハンドル番号
	//戻値：高さ
	float GetHeight(int handle);

	//画像がクリックされているか
	//引数：handle		ハンドル番号
	//戻値：されていればtrue　されていなければfalse 
	bool IsClicked(int handle);

	//解放
	void Release();
};
