#pragma once
#include <string>
#include <list>
#include <vector>

#include "Transform.h"
#include "ICollider.h"

//�Q�[���I�u�W�F�N�g�̒��ۃN���X
class IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

//�ϐ�
private:

	char state_;							//�X�e�[�^�X
	std::list<IGameObject*>	childList_;		//�q���̃��X�g
	std::vector<ICollider*>	colliderList_;	//�����蔻��̃��X�g

protected:

	std::string				objectName_;	//���O
	IGameObject*			pParent_;		//�e�̃|�C���^
	Transform				transform_;		//�ό`�����܂Ƃ߂�����

//�֐�
public:

	//�R���X�g���N�^
	//�e�N���X�̃|�C���^�Ǝ����̖��O��ݒ�
	IGameObject();
	IGameObject(IGameObject* parent);
	IGameObject(IGameObject* parent, const std::string& name);

	//list<IGameObject*>����q�N���X�̃f�X�g���N�^���Ăׂ�悤��virtual
	virtual ~IGameObject();



	//�Q�[�����[�v
	void UpdateSub();						//������Update()�Ǝq��UpdateSub()���Ă�
	void DrawSub();							//������Draw()�Ǝq��DrawSub()���Ă�
	void ReleaseSub();						//������Release()�Ǝq��ReleaseSub()���Ă�



	//�����Ǝq���̏Փˏ���
	//����:pTarget		������s���I�u�W�F�N�g
	void CollisionSub(IGameObject* pTarget);



	//��ԕω�
	void KillMe();							//���S(�폜�Ώ�)�ɂ���
	void Activate();						//Update������
	void Inactivate();						//Update�����Ȃ�
	void Visible();							//Draw������
	void Invisible();						//Draw�����Ȃ�
	void EndGame();							//�Q�[�����I������
	bool IsDead() const;					//�폜���邩
	bool IsActive() const;					//Update���s���邩
	bool IsVisible() const;					//Draw���s���邩
	bool IsEndGame() const;					//�Q�[�����I�����邩



	//���O�̈�v����I�u�W�F�N�g��T��
	//�����Fname�@�I�u�W�F�N�g��
	//�ߒl�F���������I�u�W�F�N�g�̃|�C���^
	IGameObject* FindObject(const std::string& name);			//�S�ẴI�u�W�F�N�g����
	IGameObject* FindChildObject(const std::string& name);		//�q������
	IGameObject* FindAncestrsObject(const std::string& name);	//��c����


	//�������̎擾�֐�
	IGameObject*				GetRootJob();					//���[�g�̃|�C���^
	std::list<IGameObject*>*	GetChildList();					//�q���̃��X�g
	IGameObject*				GetParent();					//�e�̃|�C���^
	std::string					GetName() const;				//�I�u�W�F�N�g��
	XMVECTOR					GetPosition() const;			//�ʒu
	XMVECTOR					GetQuaternion() const;			//��]
	XMVECTOR 					GetScale() const;				//�g��k��
	Transform*					GetTransform();					//�ό`���


	//�ʒu�ݒ�
	//����:position		�ʒu�x�N�g��
	//����:x			x���W
	//����:y			y���W
	//����:z			z���W
	void SetPosition(const XMVECTOR& position);
	void SetPosition(float x, float y, float z);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);


	//��]�ݒ�
	//����:rotate		��]�x�N�g��
	//����:x			x���̊p�x
	//����:y			y���̊p�x
	//����:z			z���̊p�x
	void SetRotate(const XMVECTOR& rotate);
	void SetRotate(float x, const float y, const float z);
	void SetRotateX(float x);
	void SetRotateY(float y);
	void SetRotateZ(float z);

	//�g��ݒ�
	//����:scale		�g��k���x�N�g��
	//����:x			x�������̊g�嗦
	//����:y			y�������̊g�嗦
	//����:z			z�������̊g�嗦
	void SetScale(const XMVECTOR& scale);
	void SetScale(float x, float y, float z);


	//�����蔻��̒ǉ�
	//�����Fcollider�@�����蔻��
	//�ߒl�F�����蔻��̃n���h��
	int AddCollider(ICollider* collider);

	//�S�Ă̓����蔻��̗L����
	void AllColliderActivate();

	//�S�Ă̓����蔻��̖�����
	void AllColliderInactivate();

protected:

	//�������z�֐�
	virtual void Initialize()	= 0;		//������
	virtual void Update()		= 0;		//�X�V
	virtual void Draw()			= 0;		//�`��
	virtual void Release()		= 0;		//���

	//�q���̊J��
	void ReleaseAllChildren();

	//�q���̕`����s������ɍs���`�揈��
	virtual void DrawAfterChildren();

	//�����������̏���
	//����:pTarget		���������I�u�W�F�N�g
	//����:pData		���������Ƃ��̃f�[�^
	virtual void OnCollision(IGameObject* pTarget, CollisionData *pData);

	//�����蔻��̈ʒu��ݒ�
	//�����P�FhCollider	�ݒ肷�铖���蔻��̃n���h��
	//�����Q�Fposision	�ݒ肷��ʒu�̔z��
	void SetColliderPosition(int hCollider, XMVECTOR const* posision);

	//�����蔻��̈ʒu��ݒ�
	//�����P�FhCollider	�ݒ肷�铖���蔻��̃n���h��
	//�����Q�Fsize		�����蔻��̑傫��
	void SetColliderSize(int hCollider, XMVECTOR const& size);

	//�q�N���X�̃C���X�^���X��
	//�����F�e�ɂȂ�IGameObject�̃|�C���^
	//�ߒl�F�������ꂽ�N���X�̃C���X�^���X
	template <class T>
	IGameObject* Instantiate(IGameObject* parent)
	{
		T* pNewObject;														//�V�����I�u�W�F�N�g�̃|�C���^
		pNewObject = new T(parent);											//�C���X�^���X��
		pNewObject->Initialize();											//������
		pNewObject->GetTransform()->SetParent(parent->GetTransform());		//�e�̕ό`����ݒ�

		parent->GetChildList()->push_back(pNewObject);						//�C���X�^���X�����ꂽ�I�u�W�F�N�g��e�̎q�����X�g�ɒǉ�����
		return pNewObject;
	}

private:

	//�����蔻��
	void Collision();						//����V�[�����̓����蔻��
};

