#pragma once
#include "../Engine/IUserInterface.h"

//タイトル画像
class TitleImage : public IUserInterface
{
public:

	TitleImage();

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};