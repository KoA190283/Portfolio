#include "ICollider.h"

#include "Global.h"
#include "Transform.h"

//�R���X�g���N�^
ICollider::ICollider(const XMVECTOR* position, const XMVECTOR& colliderSize, COLLIDER_TYPE type, int const positionSize):
	size_(colliderSize),
	type_(type), 
	isInactive_(false),
	isContinuous_(false)
{
	pPrevPosition_	= new XMVECTOR[positionSize];
	pNextPosition_	= new XMVECTOR[positionSize];

	positionSize_	= positionSize;

	SetPosision(position);
	SetPosision(position);
}

//�f�X�g���N�^
ICollider::~ICollider()
{
	SAFE_DELETE_ARRAY(pNextPosition_);
	SAFE_DELETE_ARRAY(pPrevPosition_);
}

//�����蔻��̈ʒu���X�V
void ICollider::UpdateGameObjectPosition(const XMVECTOR& posision)
{
	prevGameObjectPosition_	= nextGameObjectPosition_;	//�O�t���[���̈ʒu�����ւ�
	nextGameObjectPosition_ = posision;					//���t���[���̈ʒu���擾
}

//�����蔻��̍��W��ݒ�
void ICollider::SetPosision(const XMVECTOR* positionList)
{
	if (positionList == nullptr) return;		//�|�C���^�̒��g���Ȃ���Ή������Ȃ�

	for (int i = 0; i < positionSize_; i++)
	{
		pPrevPosition_[i] = pNextPosition_[i];	//�O�̈ʒu�����ւ�
		pNextPosition_[i] = positionList[i];	//���̈ʒu�����ւ�
	}
}

//�����蔻��̑傫����ݒ�
void ICollider::SetSize(const XMVECTOR& size)
{
	size_ = size;
}

//�����蔻��̌^���擾
COLLIDER_TYPE ICollider::GetType() const
{
	return type_; 
}

//�����蔻��ɕK�v�Ȉʒu���̐�
int ICollider::GetNeedPositionSize() const
{
	return positionSize_; 
}

//�����蔻��̗L����
void ICollider::Activate()
{
	isInactive_ = false; 
}

//�����蔻��̖�����
void ICollider::Inactivate()
{
	isInactive_ = true; 
}

//�����蔻���A���I�ɍs��
void ICollider::Continuous()
{
	isContinuous_ = true;
}

//�����蔻���f���I�ɍs��
void ICollider::Discrete()
{
	isContinuous_ = false; 
}

//�����蔻��(���^���m)
bool ICollider::IsHitSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;
	;
	//�����蔻��̂ǂ��炩��������Ԃł���Δ���͍s��Ȃ�
	if (sphere1.isInactive_ || sphere2.isInactive_)	return false;

	pData->direction					= DirectX::g_XMFltMax.v;							//�����蓯�m�̋������ő�l��
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSpheres(sphere1, sphere2, pData, 1.f);		//���̃t���[���̈ʒu�̓����蔻��

	//���m�������߂Ȃ��Ȃ�isHitNextPos��Ԃ�
	if (!(sphere1.isContinuous_ | sphere2.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//�����蔻��̋����� �ړ�����+����͈� ��藣��Ă����瓖����Ȃ�
	{
		const XMVECTOR NEXT_SPHERE_POSITION_1 = sphere1.pNextPosition_[0] + sphere1.nextGameObjectPosition_;			//sphere1�̎��̈ʒu
		const XMVECTOR PREV_SPHERE_POSITION_1 = sphere1.pPrevPosition_[0] + sphere1.prevGameObjectPosition_;			//sphere1�̑O�̈ʒu		
		const XMVECTOR NEXT_SPHERE_POSITION_2 = sphere2.pNextPosition_[0] + sphere2.nextGameObjectPosition_;			//sphere2�̎��̈ʒu
		const XMVECTOR PREV_SPHERE_POSITION_2 = sphere2.pPrevPosition_[0] + sphere2.prevGameObjectPosition_;			//sphere2�̑O�̈ʒu

		const float MOVE_SPHERE_DISTANCE_1	  = XMVector3Length(NEXT_SPHERE_POSITION_1 - PREV_SPHERE_POSITION_1).vecX;	//sphere1�̈ړ�����
		const float MOVE_SPHERE_DISTANCE_2	  = XMVector3Length(NEXT_SPHERE_POSITION_2 - PREV_SPHERE_POSITION_2).vecX;	//sphere2�̈ړ�����
		const float CLLIDER_SIZE			  = sphere1.size_.vecX + sphere2.size_.vecX;								//�����蔻��̃T�C�Y
		const float POSSIBILITY_DISTANCE	  = MOVE_SPHERE_DISTANCE_1 + MOVE_SPHERE_DISTANCE_2 + CLLIDER_SIZE;			//�Փ˂̉\�������鋗��

		//�Փ˂̉\�����Ȃ���ΏI���
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//�����蓯�m�̋������ő�l��

	//�O�t���[�����琸�x���ړ�������Ԃ��瓖���蔻��
	return JudgeSpheres(sphere1, sphere2, pData, PRECITION);
}

//�����蔻��(���^�ƃJ�v�Z���^)
bool ICollider::IsHitSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;

	//�����蔻��̂ǂ��炩��������Ԃł���Δ���͍s��Ȃ�
	if (sphere.isInactive_ || capsule.isInactive_)	return false;

	pData->direction					= g_XMFltMax.v;											//�����蓯�m�̋������ő�l��
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSphereVsCapsule(sphere, capsule, pData, 1.f);	//���̃t���[���̈ʒu�̓����蔻��

	//���m�������߂Ȃ��Ȃ�isHitNextPos��Ԃ�
	if (!(sphere.isContinuous_ | capsule.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//�����蔻��̋����� �ړ�����+����͈� ��藣��Ă����瓖����Ȃ�
	{
		const XMVECTOR	NEXT_SPHERE_POSITION		= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;						//sphere�̎��̈ʒu
		const XMVECTOR	PREV_SPHERE_POSITION		= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;						//sphere�̑O�̈ʒu
		const XMVECTOR	NEXT_CAPSULE_POSITION_FRONT	= capsule.pNextPosition_[0] + capsule.nextGameObjectPosition_;						//capsule�̎��̈ʒu�̎�O��
		const XMVECTOR	PREV_CAPSULE_POSITION_FRONT = capsule.pPrevPosition_[0] + capsule.prevGameObjectPosition_;						//capsule�̑O�̈ʒu�̎�O��
		const XMVECTOR	NEXT_CAPSULE_POSITION_BACK	= capsule.pNextPosition_[1] + capsule.nextGameObjectPosition_;						//capsule�̎��̈ʒu�̉���
		const XMVECTOR	PREV_CAPSULE_POSITION_BACK	= capsule.pPrevPosition_[1] + capsule.prevGameObjectPosition_;						//capsule�̑O�̈ʒu�̉���
	
		const float		MOVE_SPHERE_DISTANCE		= XMVector3Length(NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION).vecX;				//sphere�̈ړ�����
		const float		MOVE_CAPSULE_DISTANCE_FRONT = XMVector3Length(NEXT_CAPSULE_POSITION_FRONT - PREV_CAPSULE_POSITION_FRONT).vecX;	//capsule�̎�O���̈ړ�����
		const float		MOVE_CAPSULE_DISTANCE_BACK	= XMVector3Length(NEXT_CAPSULE_POSITION_BACK - PREV_CAPSULE_POSITION_BACK).vecX;	//capsule�̉����̈ړ�����
		const float		MOVE_CAPSULE_DISTANCE		= MOVE_CAPSULE_DISTANCE_FRONT + MOVE_CAPSULE_DISTANCE_BACK;							//capsule�̈ړ�����
		const float		CLLIDER_SIZE				= sphere.size_.vecX + capsule.size_.vecX;											//��̓����蔻��̃T�C�Y
		const float		POSSIBILITY_DISTANCE		= MOVE_SPHERE_DISTANCE + MOVE_CAPSULE_DISTANCE + CLLIDER_SIZE;						//�Փ˂̉\�������鋗��
		
		//�Փ˂̉\�����Ȃ���ΏI���
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//�����蓯�m�̋������ő�l��

	//�O�t���[�����琸�x���ړ�������Ԃ��瓖���蔻��
	return JudgeSphereVsCapsule(sphere, capsule, pData, PRECITION);
}

//�����蔻��(���^�Ɣ��^)
bool ICollider::IsHitSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;

	//�����蔻��̂ǂ��炩��������Ԃł���Δ���͍s��Ȃ�
	if (sphere.isInactive_ || box.isInactive_)	return false;

	pData->direction					= g_XMFltMax.v;									//�����蓯�m�̋������ő�l��
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSphereVsBox(sphere, box, pData, 1.f);	//���̃t���[���̈ʒu�̓����蔻��

	//���m�������߂Ȃ��Ȃ�isHitNextPos��Ԃ�
	if (!(sphere.isContinuous_ | box.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//�����蔻��̋����� �ړ�����+����͈� ��藣��Ă����瓖����Ȃ�
	{
		const XMVECTOR	NEXT_SPHERE_POSITION = sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;			//sphere�̎��̈ʒu
		const XMVECTOR	PREV_SPHERE_POSITION = sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;			//sphere�̑O�̈ʒu
		const XMVECTOR	NEXT_BOX_POSITION	 = box.pNextPosition_[0] + box.nextGameObjectPosition_;					//box�̎��̈ʒu
		const XMVECTOR	PREV_BOX_POSITION	 = box.pPrevPosition_[0] + box.prevGameObjectPosition_;					//box�̑O�̈ʒu

		const float		MOVE_SPHERE_DISTANCE = XMVector3Length(NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION).vecX;	//sphere�̈ړ�����
		const float		MOVE_BOX_DISTANCE	 = XMVector3Length(NEXT_BOX_POSITION - PREV_BOX_POSITION).vecX;			//box�̈ړ�����
		const float		CLLIDER_SIZE		 = sphere.size_.vecX + XMVector3Length(box.size_).vecX;					//box�̈ړ�����

		const float		POSSIBILITY_DISTANCE = MOVE_SPHERE_DISTANCE + MOVE_BOX_DISTANCE + CLLIDER_SIZE;				//�Փ˂̉\��������͈�

		//�Փ˂̉\�����Ȃ���ΏI���
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//�����蓯�m�̋������ő�l��

	//�O�t���[�����琸�x���ړ�������Ԃ��瓖���蔻��
	return JudgeSphereVsBox(sphere, box, pData, PRECITION);
}

//���菈��(���^���m)
bool ICollider::JudgeSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;

	//100%�𒴂��Ă���ΏՓ˔���I��
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//�O�t���[������percentage�����i�񂾏�ԂŏՓ˔���
	{
		//�����t���[���O��̔���Ɏg�p����ʒu
		const XMVECTOR NEXT_SPHERE_POSITION_1	= sphere1.pNextPosition_[0] + sphere1.nextGameObjectPosition_;	//sphere1�̎��̈ʒu
		const XMVECTOR PREV_SPHERE_POSITION_1	= sphere1.pPrevPosition_[0] + sphere1.prevGameObjectPosition_;	//sphere1�̑O�̈ʒu		
		const XMVECTOR NEXT_SPHERE_POSITION_2	= sphere2.pNextPosition_[0] + sphere2.nextGameObjectPosition_;	//sphere2�̎��̈ʒu
		const XMVECTOR PREV_SPHERE_POSITION_2	= sphere2.pPrevPosition_[0] + sphere2.prevGameObjectPosition_;	//sphere2�̑O�̈ʒu
	
		//�t���[���Ԃ̈ړ��x�N�g��
		const XMVECTOR MOVE_SPHERE_1			= NEXT_SPHERE_POSITION_1 - PREV_SPHERE_POSITION_1;				//sphere1�̈ړ��x�N�g��
		const XMVECTOR MOVE_SPHERE_2			= NEXT_SPHERE_POSITION_2 - PREV_SPHERE_POSITION_2;				//sphere2�̈ړ��x�N�g��

		//�O�t���[������percentage�����i�񂾈ʒu
		const XMVECTOR SPHERE_POSITION_1		= PREV_SPHERE_POSITION_1 + MOVE_SPHERE_1 * percentage;			//sphere1�̈ʒu
		const XMVECTOR SPHERE_POSITION_2		= PREV_SPHERE_POSITION_2 + MOVE_SPHERE_2 * percentage;			//sphere2�̈ʒu

		//�O���߂Â��Ă��Ȃ���ΏՓ˂��Ȃ�
		const XMVECTOR COLLISION_DIRECTION		= SPHERE_POSITION_1 - SPHERE_POSITION_2;						//sphere2����sphere1�ւ̓����蔻��̈ʒu�ւ̃x�N�g��
		const float	   NEXT_COLLIDERS_DISTANCE	= XMVector3Length(COLLISION_DIRECTION).vecX;					//��̓����蔻��̋���
		const float	   PREV_COLLIDERS_DISTANCE	= XMVector3Length(pData->direction).vecX;						//�O�̓�̓����蔻��̋���
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;

		//�����蔻��̃f�[�^���X�V
		pData->positionList = { SPHERE_POSITION_1,SPHERE_POSITION_2};											//�����蔻��̈ʒu
		pData->direction	= COLLISION_DIRECTION;																//�����蔻��Ԃ̃x�N�g��

		//�����蔻�蓯�m�̋����������蔻��̃T�C�Y���Z����Γ������Ă���
		float COLLIDER_SIZE = sphere1.size_.vecX + sphere2.size_.vecX;											//��̓����蔻��̃T�C�Y
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE) return true;
	}

	//���x���i�߂čēx����
	return JudgeSpheres(sphere1, sphere2, pData, percentage + PRECITION);
}

//���菈��(���^�ƃJ�v�Z���^) 
bool ICollider::JudgeSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;
	using DirectX::XMVector3Normalize;

	//100%�𒴂��Ă���ΏՓ˔���I��
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//�O�t���[������percentage�����i�񂾏�ԂŏՓ˔���
	{
		//�����t���[���O��̔���Ɏg�p����ʒu
		const XMVECTOR	NEXT_SPHERE_POSITION		= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;					//sphere1�̎��̈ʒu
		const XMVECTOR	PREV_SPHERE_POSITION		= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;					//sphere1�̑O�̈ʒu		
		const XMVECTOR	NEXT_CAPSULE_POSITION_FRONT	= capsule.pNextPosition_[0] + capsule.nextGameObjectPosition_;					//capsule�̎��̈ʒu�̎�O��
		const XMVECTOR	PREV_CAPSULE_POSITION_FRONT = capsule.pPrevPosition_[0] + capsule.prevGameObjectPosition_;					//capsule�̑O�̈ʒu�̎�O��
		const XMVECTOR	NEXT_CAPSULE_POSITION_BACK	= capsule.pNextPosition_[1] + capsule.nextGameObjectPosition_;					//capsule�̎��̈ʒu�̉���
		const XMVECTOR	PREV_CAPSULE_POSITION_BACK	= capsule.pPrevPosition_[1] + capsule.prevGameObjectPosition_;					//capsule�̑O�̈ʒu�̉���

		//�t���[���Ԃ̈ړ��x�N�g��
		const XMVECTOR	MOVE_SPHERE					= NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION;									//sphere�̈ړ��x�N�g��
		const XMVECTOR	MOVE_CAPSULE_FRONT			= PREV_CAPSULE_POSITION_FRONT - PREV_CAPSULE_POSITION_FRONT;					//capsule�̎�O���̈ړ��x�N�g��
		const XMVECTOR	MOVE_CAPSULE_BACK			= NEXT_CAPSULE_POSITION_BACK - PREV_CAPSULE_POSITION_BACK;						//capsule�̉����̈ړ��x�N�g��

		//�O�t���[������percentage�����i�񂾈ʒu
		const XMVECTOR	SPHERE_POSITION				= PREV_SPHERE_POSITION + MOVE_SPHERE * percentage;								//sphere�̈ʒu
		const XMVECTOR	CAPSULE_POSITION_FRONT		= PREV_CAPSULE_POSITION_FRONT + MOVE_CAPSULE_FRONT * percentage;				//capsule�̎�O���̈ʒu
		const XMVECTOR	CAPSULE_POSITION_BACK		= PREV_CAPSULE_POSITION_BACK + MOVE_CAPSULE_BACK * percentage;					//capsule�̉����̈ʒu

		//capsulePos1����ɂ����x�N�g��
		const XMVECTOR	TO_SPHERE					= SPHERE_POSITION - CAPSULE_POSITION_FRONT;										//SPHERE_POSITION�ւ̃x�N�g��
		const XMVECTOR	TO_CAPSULE_BACK				= CAPSULE_POSITION_BACK - CAPSULE_POSITION_FRONT;								//CAPSULE_POSITION_BACK�ւ̃x�N�g��
		
		//���K��
		const XMVECTOR	TO_SPHERE_DIRECTION			= XMVector3Normalize(TO_SPHERE);												//TO_SPHERE�𐳋K����������
		const XMVECTOR	TO_CAPSULE_BACK_DIRECTION	= XMVector3Normalize(TO_CAPSULE_BACK);											//TO_CAPSULE_BACK�𐳋K����������

		//capsule�̒�����ɂ���sphere�ƍł��߂��ʒu���v�Z
		const float		DOT							= DirectX::XMVector3Dot(TO_SPHERE_DIRECTION, TO_CAPSULE_BACK_DIRECTION).vecX;	//�{�[���ւ̃x�N�g���ƃJ�v�Z���̉��̈ʒu�ւ̃x�N�g���̓���
		XMVECTOR		collisionPosition			= TO_CAPSULE_BACK_DIRECTION * XMVector3Length(TO_CAPSULE_BACK) * DOT;			//capsule�̒������sphere�ƍł��߂��ʒu
		
		//sphere��CAPSULE_POSITION_FRONT����O�ɂ���Ȃ瓖���蔻��̈ʒu����_��
		if (DOT < 0) collisionPosition	= DirectX::g_XMZero;

		//capsule�̓����蔻��̈ʒu��capsule�x�N�g���͈̔͊O�Ȃ�capsule�̓����蔻��̈ʒu��capsule�x�N�g���̐�[�ʒu��
		const bool IS_RANGE = 0 < XMVector3Length(TO_CAPSULE_BACK).vecX - XMVector3Length(collisionPosition).vecX;					//capsule�̓����蔻��x�N�g���͈͓̔���
		if (!IS_RANGE) collisionPosition = TO_CAPSULE_BACK;

		//�O���߂Â��Ă��Ȃ���΂���ȍ~�͏Փ˂��Ȃ�
		const XMVECTOR	COLLISION_DIRECTION			= collisionPosition - TO_SPHERE;												//capsule����sphere�ւ̓����蔻��̈ʒu�ւ̃x�N�g��
		const float		NEXT_COLLIDERS_DISTANCE		= XMVector3Length(COLLISION_DIRECTION).vecX;									//��̓����蔻��̋���
		const float		PREV_COLLIDERS_DISTANCE		= XMVector3Length(pData->direction).vecX;										//�O�̓�̓����蔻��̋���
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;
			
		//�����蔻��̃f�[�^���X�V
		pData->positionList = { SPHERE_POSITION,CAPSULE_POSITION_FRONT,CAPSULE_POSITION_BACK};										//�����蔻��̈ʒu
		pData->direction	= COLLISION_DIRECTION;																					//�����蔻��Ԃ̃x�N�g��
			
		//�����蔻�蓯�m�̋����������蔻��̃T�C�Y���Z����Γ������Ă���
		const float COLLIDER_SIZE = sphere.size_.vecX + capsule.size_.vecX;															//��̓����蔻��̃T�C�Y
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE) return true;
	}

	//���x���i�߂čēx����
	return JudgeSphereVsCapsule(sphere, capsule, pData, percentage + PRECITION);
}

bool ICollider::JudgeSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;

	//100%�𒴂��Ă���ΏՓ˔���I��
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//�O�t���[������percentage�����i�񂾏�ԂŏՓ˔���
	{
		//�����t���[���O��̔���Ɏg�p����ʒu
		const XMVECTOR	NEXT_SPHERE_POSITION	= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;	//sphere�̎��̈ʒu
		const XMVECTOR	PREV_SPHERE_POSITION	= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;	//sphere�̑O�̈ʒu
		const XMVECTOR	NEXT_BOX_POSITION		= box.pNextPosition_[0] + box.nextGameObjectPosition_;			//box�̎��̈ʒu
		const XMVECTOR	PREV_BOX_POSITION		= box.pPrevPosition_[0] + box.prevGameObjectPosition_;			//box�̑O�̈ʒu

		//�t���[���Ԃ̈ړ��x�N�g��
		const XMVECTOR	MOVE_SPHERE = NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION;								//sphere�̈ړ�����
		const XMVECTOR	MOVE_BOX	= NEXT_BOX_POSITION - PREV_BOX_POSITION;									//box�̈ړ�����

		//�O�t���[������percentage�����i�񂾈ʒu
		const XMVECTOR	SPHERE_POSITION = PREV_SPHERE_POSITION + MOVE_SPHERE * percentage;						//sphere�̈ʒu
		const XMVECTOR	BOX_POSITION	= PREV_BOX_POSITION + MOVE_BOX * percentage;							//box�̐�[�̈ʒu

		//���^�����蔻��͈̔�
		XMVECTOR boxMax, boxMin;
		boxMax.vecX = BOX_POSITION.vecX + box.size_.vecX;														//box��X�����̔���͈�
		boxMax.vecY = BOX_POSITION.vecY + box.size_.vecY;														//box��Y�����̔���͈�		
		boxMax.vecZ = BOX_POSITION.vecZ + box.size_.vecZ;														//box��Z�����̔���͈�	
		boxMin.vecX = BOX_POSITION.vecX - box.size_.vecX;														//box��X�����̔���͈�
		boxMin.vecY = BOX_POSITION.vecY - box.size_.vecY;														//box��Y�����̔���͈�		
		boxMin.vecZ = BOX_POSITION.vecZ - box.size_.vecZ;														//box��Z�����̔���͈�	

		XMVECTOR collisionPosition = SPHERE_POSITION;															//box�ʏ��collisionPos����ł��߂��_															

		//collisionPos��box�ʏ�ɂ��邩���m�F����
		XMVECTOR boxLimit[2] = { boxMin, boxMax };
		if (collisionPosition.vecX < boxMin.vecX || boxMax.vecX < collisionPosition.vecX)						//X����
			collisionPosition.vecX = boxLimit[0 < collisionPosition.vecX].vecX;

		if (collisionPosition.vecZ < boxMin.vecZ || boxMax.vecZ < collisionPosition.vecZ)						//Z����
			collisionPosition.vecZ = boxLimit[0 < collisionPosition.vecZ].vecZ;

		if (collisionPosition.vecY < boxMin.vecY || boxMax.vecY < collisionPosition.vecY)						//Y����
			collisionPosition.vecY = boxLimit[0 < collisionPosition.vecY].vecY;

		//�O���߂Â��Ă��Ȃ���΂���ȍ~�͏Փ˂��Ȃ�
		const XMVECTOR	COLLISION_DIRECTION		= collisionPosition - SPHERE_POSITION;							//sphere����capsule�ւ̓����蔻��̈ʒu�ւ̃x�N�g��
		const float		NEXT_COLLIDERS_DISTANCE = XMVector3Length(COLLISION_DIRECTION).vecX;					//��̓����蔻��̋���
		const float		PREV_COLLIDERS_DISTANCE = XMVector3Length(pData->direction).vecX;						//�O�̓�̓����蔻��̋���
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;

		//�����蔻��̃f�[�^���X�V
		pData->positionList = { SPHERE_POSITION,BOX_POSITION };													//�����蔻��̈ʒu
		pData->direction	= COLLISION_DIRECTION;																//�����蔻��Ԃ̃x�N�g��

		//�����蔻�蓯�m�̋����������蔻��̃T�C�Y���Z����Γ������Ă���
		const float COLLIDER_SIZE = sphere.size_.vecX;															//��̓����蔻��̃T�C�Y
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE)
		{
			return true;
		}
	}

	//���x���i�߂čēx����
	return JudgeSphereVsBox(sphere, box, pData, percentage + PRECITION);
}