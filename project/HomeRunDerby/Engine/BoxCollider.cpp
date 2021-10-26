#include "BoxCollider.h"

#include "Model.h"

//�R���X�g���N�^
BoxCollider::BoxCollider(const XMVECTOR* position, const XMVECTOR& size):
	ICollider(position, size, COLLIDER_TYPE::BOX, 1)
{
}

//����������
bool BoxCollider::IsHit(const ICollider& target, CollisionData* data)
{
	//����̓����蔻��^
	COLLIDER_TYPE targetType = target.GetType();

	//����̌^�ɂ��������菈��
	switch (targetType)
	{
	case COLLIDER_TYPE::SPHERE:	//��
		return IsHitSphereVsBox(target, *this, data);

	default:					//����`
		return false;
	}

	return false;
}