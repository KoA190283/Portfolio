#pragma once
#include "../Engine/IGameObject.h"

//フィールドの管理
class Field : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//コンストラクタ
	Field(IGameObject* parent);

	//初期化
	void Initialize() override;

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};