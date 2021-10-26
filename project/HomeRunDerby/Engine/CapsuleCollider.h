#pragma once
#include "ICollider.h"

//カプセル型当たり判定
class CapsuleCollider : public ICollider
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//コンストラクタ
	//引数１：position　当たり判定の中心位置
	//引数２：size		当たり判定の大きさ
	CapsuleCollider(const XMVECTOR* position, const XMVECTOR& size);

	//当たったか
	//引数１：target	判定する相手の当たり判定
	//引数２：pData		衝突判定の情報
	bool IsHit(const ICollider& target, CollisionData* data) override;
};

