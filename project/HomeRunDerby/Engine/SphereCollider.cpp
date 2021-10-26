#include "SphereCollider.h"

#include "Model.h"

//�R���X�g���N�^
SphereCollider::SphereCollider(const XMVECTOR* position, const XMVECTOR& size) :
	ICollider(position, size, COLLIDER_TYPE::SPHERE, 1)
{
}

//����������
bool SphereCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//����̓����蔻��^
	COLLIDER_TYPE targetType = target.GetType();

	//����̌^�ɂ��������菈��
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:		//��
		return IsHitSpheres(*this, target, data);

	case COLLIDER_TYPE::CAPSULE:	//�J�v�Z��
		return IsHitSphereVsCapsule(*this, target, data);

	case COLLIDER_TYPE::BOX:		//��
		return IsHitSphereVsBox(*this, target, data);

	default:						//����`
		return false;
	}

	return false;
}