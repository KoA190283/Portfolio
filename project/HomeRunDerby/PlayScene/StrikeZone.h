#pragma once
#include "../Engine/IGameObject.h"

//ボールの管理
class StrikeZone : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//コンストラクタ
	StrikeZone(IGameObject* parent);

	//初期化
	void Initialize() override;

	//他オブジェクトと衝突した時の処理
	//引数１：pTarget		当たったオブジェクト
	//引数２：pData			衝突時のデータ
	void OnCollision(IGameObject* pTarget, CollisionData *pData) override;

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;

	//押された（クリックされた）ときの処理
	void Pressed();

	//的になるボールの描画
	void DrawTagetBall();
};