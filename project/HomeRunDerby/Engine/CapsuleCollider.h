#pragma once
#include "ICollider.h"

//�J�v�Z���^�����蔻��
class CapsuleCollider : public ICollider
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//�R���X�g���N�^
	//�����P�Fposition�@�����蔻��̒��S�ʒu
	//�����Q�Fsize		�����蔻��̑傫��
	CapsuleCollider(const XMVECTOR* position, const XMVECTOR& size);

	//����������
	//�����P�Ftarget	���肷�鑊��̓����蔻��
	//�����Q�FpData		�Փ˔���̏��
	bool IsHit(const ICollider& target, CollisionData* data) override;
};

