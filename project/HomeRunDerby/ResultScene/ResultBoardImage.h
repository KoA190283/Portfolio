#pragma once
#include "../Engine/IUserInterface.h"

//結果発表ボード画像
class ResultBoardImage : public IUserInterface
{
public:

	ResultBoardImage();

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};
