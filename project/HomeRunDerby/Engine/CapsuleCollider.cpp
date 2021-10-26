#include "CapsuleCollider.h"

#include "Model.h"

//コンストラクタ
CapsuleCollider::CapsuleCollider(const XMVECTOR* position, const XMVECTOR& size) :
	ICollider(position, size, COLLIDER_TYPE::CAPSULE, 2)
{
}

//当たったか
bool CapsuleCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//相手の当たり判定型
	COLLIDER_TYPE targetType = target.GetType();

	//相手の型にあった判定処理
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:	//球
		return IsHitSphereVsCapsule(target, *this, data);

	case COLLIDER_TYPE::CAPSULE://カプセル
		return IsHitSphereVsCapsule(*this, target, data);

	default:					//未定義
		return false;
	}

	return false;
}