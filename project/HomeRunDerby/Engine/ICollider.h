#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <list>

//�����蔻��̌^
enum class COLLIDER_TYPE 
{
	SPHERE = 0,				//��
	CAPSULE,				//�J�v�Z��
	BOX,					//��
	MAX
};

//�Փˎ��̃f�[�^
//(���^�F�����@�ʌ^�FCOLLIDER_TYPE�Ő�ɒ�`���ꂽ��) ����
struct CollisionData 
{
	using XMVECTOR = DirectX::XMVECTOR;

	std::list<XMVECTOR>	 positionList;	//�����Ƒ���̏Փˎ��̈ʒu�@(���^�F�����@�ʌ^�FCOLLIDER_TYPE�Ő�ɒ�`���ꂽ��) ����i�[
	XMVECTOR direction;					//(���^�F�����@�ʌ^�FCOLLIDER_TYPE�Ő�ɒ�`���ꂽ��) ���� ����ւ̃x�N�g��

	CollisionData():direction(DirectX::g_XMZero){}
};

const float PRECITION = 0.01f;			//�A���I�ɓ����蔻����s���ꍇ�̐��x

//�����蔻��̒��ۃN���X
class ICollider
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMVECTOR*				pPrevPosition_;				//�I�u�W�F�N�g���猩�������蔻��̈ʒu
	XMVECTOR*				pNextPosition_;				//�I�u�W�F�N�g���猩�������蔻��̈ʒu
	XMVECTOR				prevGameObjectPosition_;	//�O�t���[���̓����蔻��̈ʒu
	XMVECTOR				nextGameObjectPosition_;	//���t���[���̓����蔻��̈ʒu
	XMVECTOR				size_;						//�����蔻��̑傫��
	COLLIDER_TYPE			type_;						//�����蔻��̌^
	int						positionSize_;				//����Ɏg�p����ʒu�̐�
	bool					isInactive_;				//�����蔻����s����
	bool					isContinuous_;				//�A���I�ɓ����蔻����s�����i���蔲���h�~�j

public:

	//�R���X�g���N�^
	//�����P�Fposition�@	�����蔻��̈ʒu
	//�����Q�FcolliderSize�@�����蔻��̑傫���i���a�j
	//�����R�Ftype�@		�����蔻��̌^
	//�����S�FposisionSize�@����Ɏg�p����ʒu�̐�
	ICollider(XMVECTOR const* position, XMVECTOR const& colliderSize, COLLIDER_TYPE type, int const positionSize);

	//�f�X�g���N�^
	virtual ~ICollider();

	//�������z�֐�
	//�����������ǂ���
	//�����P�FpData			�Փ˔���̏��
	//�����Q�Ftarget�@		���肷�鑊��
	//�ߒl�F����������
	virtual bool IsHit(const ICollider& target, CollisionData* pData) = 0;

	//�����蔻��̈ʒu���X�V
	//�����Fposition		�ʒu
	void UpdateGameObjectPosition(const XMVECTOR& posision);

	//�����蔻��̍��W��ݒ�
	//�����FpositionList	�ʒu�̔z��
	void SetPosision(const XMVECTOR* positionList);

	//�����蔻��̑傫����ݒ�
	//�����Fsize			
	void SetSize(const XMVECTOR& size);

	//�����蔻��̌^���擾
	//�ߒl�F�����蔻��̌^
	COLLIDER_TYPE GetType() const;

	//�����蔻��ɕK�v�Ȉʒu���̐����擾
	//�ߒl�F�����蔻��ɕK�v�Ȉʒu���̐�
	int GetNeedPositionSize() const;

	//�����蔻��̗L����
	void Activate();

	//�����蔻��̖�����
	void Inactivate();

	//�����蔻���A���I�ɍs��
	void Continuous();

	//�����蔻���f���I�ɍs��
	void Discrete();

protected:

	//�����蔻��
	//�����P�F���肷�铖���蔻��P
	//�����Q�F���肷�铖���蔻��Q
	//�����R�FpData			���莞�̏��
	//�ߒl�F����������
	bool IsHitSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData) const;							//���^���m
	bool IsHitSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData) const;						//���^�ƃJ�v�Z���^
	bool IsHitSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData) const;								//���^�Ə��^
	
private:

	//�����蔻�菈��
	//�����P�F���肷�铖���蔻��P
	//�����Q�F���肷�铖���蔻��Q
	//�����R�FpData			���莞�̏��
	//�����S�Fpercentage	����t���[���Ԃ̈ړ�����
	//�ߒl�F����������
	bool JudgeSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData, float percentage) const;			//���^���m
	bool JudgeSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData, float percentage) const;	//���^�ƃJ�v�Z���^
	bool JudgeSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData, float percentage) const;			//���^�Ɣ��^

};

