#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"

//リザルトシーンの管理
class ResultScene : public IGameObject
{
private:

	std::list<IUserInterface*>	userInterfaceList_;	//UIクラスのリスト

public:

	//コンストラクタ
	ResultScene(IGameObject* parent);

	//初期化
	void Initialize() override;

	//シーンを変更する
	void ChangeSceneTitle() const;

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};