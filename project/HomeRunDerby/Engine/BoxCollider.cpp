#include "BoxCollider.h"

#include "Model.h"

//コンストラクタ
BoxCollider::BoxCollider(const XMVECTOR* position, const XMVECTOR& size):
	ICollider(position, size, COLLIDER_TYPE::BOX, 1)
{
}

//当たったか
bool BoxCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//相手の当たり判定型
	COLLIDER_TYPE targetType = target.GetType();

	//相手の型にあった判定処理
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:	//球
		return IsHitSphereVsBox(target, *this, data);

	default:					//未定義
		return false;
	}

	return false;
}