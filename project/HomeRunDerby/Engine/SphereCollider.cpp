#include "SphereCollider.h"

#include "Model.h"

//コンストラクタ
SphereCollider::SphereCollider(const XMVECTOR* position, const XMVECTOR& size) :
	ICollider(position, size, COLLIDER_TYPE::SPHERE, 1)
{
}

//当たったか
bool SphereCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//相手の当たり判定型
	COLLIDER_TYPE targetType = target.GetType();

	//相手の型にあった判定処理
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:		//球
		return IsHitSpheres(*this, target, data);

	case COLLIDER_TYPE::CAPSULE:	//カプセル
		return IsHitSphereVsCapsule(*this, target, data);

	case COLLIDER_TYPE::BOX:		//床
		return IsHitSphereVsBox(*this, target, data);

	default:						//未定義
		return false;
	}

	return false;
}