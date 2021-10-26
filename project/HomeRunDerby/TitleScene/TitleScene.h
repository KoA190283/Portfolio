#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"

//タイトルシーンの管理
class TitleScene : public IGameObject
{
private:

	std::list<IUserInterface*> userInterfaceList_;	//UIクラスのリスト

public:

	//コンストラクタ
	TitleScene(IGameObject* parent);

	//初期化
	void Initialize() override;

	//シーンを変更する
	void ChangeNextScene() const;

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};