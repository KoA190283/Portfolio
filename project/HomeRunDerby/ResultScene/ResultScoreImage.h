#pragma once
#include "../Engine/IUserInterface.h"

//結果画像
class ResultScoreImage : public IUserInterface
{
public:

	ResultScoreImage();

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};
