#include "CapsuleCollider.h"

#include "Model.h"

//�R���X�g���N�^
CapsuleCollider::CapsuleCollider(const XMVECTOR* position, const XMVECTOR& size) :
	ICollider(position, size, COLLIDER_TYPE::CAPSULE, 2)
{
}

//����������
bool CapsuleCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//����̓����蔻��^
	COLLIDER_TYPE targetType = target.GetType();

	//����̌^�ɂ��������菈��
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:	//��
		return IsHitSphereVsCapsule(target, *this, data);

	case COLLIDER_TYPE::CAPSULE://�J�v�Z��
		return IsHitSphereVsCapsule(*this, target, data);

	default:					//����`
		return false;
	}

	return false;
}