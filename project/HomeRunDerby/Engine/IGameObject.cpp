#include "IGameObject.h"

#include "Global.h"

/*
	IGameObject::state_�̃f�[�^�i�[�ɂ���

	//�r�b�g���F�Ǘ�����t���O

	1:�I�u�W�F�N�g���폜���邩
	2:�X�V���s����
	3:�`����s����
	4:�Q�[�����I�����邩

	5:��f�[�^
	6:��f�[�^
	7:��f�[�^
	8:��f�[�^
*/

const char DEAD		 = 0x01;	//����ł�
const char INACTIVE	 = 0x02;	//�X�V���Ȃ�
const char INVISIBLE = 0x04;	//�`�悵�Ȃ�
const char END_GAME	 = 0x08;	//�Q�[���I��

using DirectX::XMVECTOR;

//�R���X�g���N�^(�����Ȃ�)
//�R���X�g���N�^��(nullptr,"")�������ɂ��ČĂяo��
IGameObject::IGameObject():IGameObject(nullptr,"")
{
}
//�R���X�g���N�^(�����F�e�|�C���^)
//�R���X�g���N�^��(�e�|�C���^,"")�������ɂ��ČĂяo��
IGameObject::IGameObject(IGameObject* parent) :
	IGameObject(parent, "")
{
}
//�R���X�g���N�^(�����F�e�|�C���^�A�I�u�W�F�N�g��)
//�e�N���X�̃|�C���^�Ǝ����̖��O��ݒ�
IGameObject::IGameObject(IGameObject* parent,std::string const & name) :
	pParent_(parent), objectName_(name), state_(0)
{
	//���X�g�̏�����
	colliderList_.clear();
	childList_.clear();	
}

//�f�X�g���N�^
IGameObject::~IGameObject()
{
}

//������Update()�Ǝq��UpdateSub()���Ă�
void IGameObject::UpdateSub()
{
	//�X�V���Ȃ�
	if (!IsActive()) return;

	//�X�V
	Update();

	//�����蔻��̈ʒu���X�V
	for (auto collider : colliderList_) collider->UpdateGameObjectPosition(transform_.position_);

	//childList_��UpdateSub���Ăяo��
	for (auto child : childList_) child->UpdateSub();

	//�X�V��̏���
	auto it = childList_.begin();
	while (it != childList_.end())
	{
		//����ł���
		if ((*it)->IsDead())
		{
			//���񂾎q���̉��
			(*it)->ReleaseSub();
			SAFE_DELETE(*it);
			it = childList_.erase(it);
		}
		//�����Ă���
		else
		{
			//�����蔻��
			(*it)->Collision();
			it++;
		}
	}

	//�q���̓����蔻��̈ʒu���X�V
	for (auto child : childList_) {
		for (auto collider : child->colliderList_)
			collider->UpdateGameObjectPosition(child->transform_.position_);
	}
}

//������Draw()�Ǝq��DrawSub()���Ă�
void IGameObject::DrawSub()
{
	//�`�悵�Ȃ�
	if (!IsVisible()) return;

	//�`��
	Draw();

	//childList_��DrawSub���Ăяo��
	for (auto child : childList_) child->DrawSub();

	//�q���̕`����s������ɍs���`�揈��
	DrawAfterChildren();
}

//������Release()�Ǝq��ReleaseSub()���Ă�
void IGameObject::ReleaseSub()
{
	//�����蔻��̉��
	for (auto collider : colliderList_) SAFE_DELETE(collider);

	colliderList_.clear();

	//childList_��ReleaseSub���Ăяo��
	ReleaseAllChildren();

	//���
	Release();
}

//�����Ǝq���̏Փˏ���
void IGameObject::CollisionSub(IGameObject* pTarget)
{
	//�q���������
	if (!childList_.empty())
	{
		//�q���̓����蔻��
		for (auto child : childList_) child->CollisionSub(pTarget);
	}

	//�����I�u�W�F�N�g���m�Ȃ画�肵�Ȃ�
	if (this == pTarget)  return;

	//�����蔻��������Ă��Ȃ���Δ��肵�Ȃ�
	if (colliderList_.empty()) return;

	//�����̎������蔻�蕪���[�v
	for (auto collider : colliderList_) {

		//�^�[�Q�b�g�̎������蔻�蕪���[�v
		for (auto targetCollider : pTarget->colliderList_) {

			CollisionData pData;

			//�������Ă���
			if (collider->IsHit(*targetCollider, &pData)) {

				//�����������̏���
				OnCollision(pTarget, &pData);
			}
		}
	}

}

//�����̏�Ԃ����S�ɂ���
void IGameObject::KillMe()
{
	state_ |= DEAD; 
}

//Update������
void IGameObject::Activate() 
{ 
	state_ &= !INACTIVE;
}

//Update�����Ȃ�
void IGameObject::Inactivate() 
{
	state_ |= INACTIVE; 
}

//Draw������
void IGameObject::Visible() 
{
	state_ &= !INVISIBLE;
}

//Draw�����Ȃ�
void IGameObject::Invisible() 
{
	state_ |= INVISIBLE;
}

//�Q�[�����I������
void IGameObject::EndGame() 
{
	GetRootJob()->state_ |= END_GAME;
}

//����ł邩
bool IGameObject::IsDead() const
{
	return state_ & DEAD;
}

//Update���s���邩
bool IGameObject::IsActive() const
{
	return !(state_ & INACTIVE);
}

//Draw���s���邩
bool IGameObject::IsVisible() const
{
	return !(state_ & INVISIBLE); 
}

//�Q�[�����I�����邩
bool IGameObject::IsEndGame() const
{
	return state_ & END_GAME;
}

//�S�ẴI�u�W�F�N�g���疼�O�̈�v������̂�T��
IGameObject* IGameObject::FindObject(const std::string & name)
{
	return GetRootJob()->FindChildObject(name);
}

//���O�̈�v����q����T��
IGameObject* IGameObject::FindChildObject(const std::string & name)
{
	size_t size = childList_.size();

	//�q����T��
	for(auto child: childList_)
	{
		//��������|�C���^��Ԃ�
		if (child->objectName_ == name) return child;

		//����T��
		IGameObject* obj = (child)->FindChildObject(name);
		
		//���������瑷��Ԃ�
		if (obj) return obj;
	}

	//������Ȃ�������nullptr��Ԃ�
	return nullptr;
}

//���O�̈�v�����c��T��
IGameObject* IGameObject::FindAncestrsObject(const std::string& name)
{
	//���O���������玩����Ԃ�
	if (objectName_ == name) return this;
	
	//�e��������e��T���ĕԂ�
	else if (pParent_ != nullptr) return pParent_->FindAncestrsObject(name);
	
	//���Ȃ�������nullptr��Ԃ�
	else return nullptr;
}

//���[�g�̃|�C���^���擾
IGameObject* IGameObject::GetRootJob()
{
	//�e�����Ȃ������玩����Ԃ�
	if (pParent_ == nullptr) return this;

	//������e�̐e��T��
	else return pParent_ -> GetRootJob();
}

//�q���̃��X�g
std::list<IGameObject*>* IGameObject::GetChildList()
{
	return &childList_; 
}				

//�e�̃|�C���^
IGameObject* IGameObject::GetParent()
{
	return pParent_;
}				

//�I�u�W�F�N�g��
std::string	IGameObject::GetName() const
{
	return objectName_; 
}				

//�ʒu
XMVECTOR IGameObject::GetPosition() const
{
	return transform_.position_; 
}	

//��]
XMVECTOR IGameObject::GetQuaternion() const
{
	return transform_.quaternion_; 
}	

//�g��k��
XMVECTOR IGameObject::GetScale() const
{
	return transform_.scale_;
}		

//�ό`���
Transform* IGameObject::GetTransform()
{
	return &transform_; 
}				

//�ʒu�ݒ�
void IGameObject::SetPosition(const XMVECTOR& position) 
{
	transform_.position_ = position;
}

//�ʒu�ݒ�
void IGameObject::SetPosition(float x, float y, float z) 
{
	SetPosition(XMVECTOR{ x, y, z }); 
}

//x�x�N�g���̈ʒu�ݒ�
void IGameObject::SetPositionX(float x)
{
	transform_.position_.vecX = x;
}

//y�x�N�g���̈ʒu�ݒ�
void IGameObject::SetPositionY(float y)
{
	transform_.position_.vecY = y;
}

//z�x�N�g���̈ʒu�ݒ�
void IGameObject::SetPositionZ(float z)
{
	transform_.position_.vecZ = z;
}

//��]�ݒ�
void IGameObject::SetRotate(const XMVECTOR& rotate) 
{
	transform_.SetRotate(rotate); 
}

//��]�ݒ�
void IGameObject::SetRotate(float x, float y, float z)
{
	SetRotate(XMVECTOR{ x, y, z }); 
}

//x�x�N�g���̉�]�ݒ�
void IGameObject::SetRotateX(float x) 
{
	SetRotate(XMVECTOR{ x, 0, 0 }); 
}

//y�x�N�g���̉�]�ݒ�
void IGameObject::SetRotateY(float y) 
{
	SetRotate(XMVECTOR{ 0, y, 0 });
}

//z�x�N�g���̉�]�ݒ�
void IGameObject::SetRotateZ(float z) 
{
	SetRotate(XMVECTOR{ 0, 0, z }); 
}

//�g��ݒ�
void IGameObject::SetScale(const XMVECTOR& scale) 
{
	transform_.scale_ = scale; 
}

//�g��ݒ�
void IGameObject::SetScale(float x, float y, float z)
{
	SetScale(DirectX::XMVectorSet(x, y, z, 0)); 
}

//�����蔻��̒ǉ�
int IGameObject::AddCollider(ICollider* collider)
{
	//�����蔻��̃��X�g�ɒǉ�
	colliderList_.push_back(collider);

	//���݂̎����̈ʒu��������
	collider -> UpdateGameObjectPosition(transform_.position_);

	//�n���h����Ԃ�
	return (int)colliderList_.size() - 1;
}

//�S�Ă̓����蔻��̗L����
void IGameObject::AllColliderActivate()
{
	if (colliderList_.empty()) return;

	for (auto collider : colliderList_)	collider->Activate();
}

//�S�Ă̓����蔻��̖�����
void IGameObject::AllColliderInactivate()
{
	if (colliderList_.empty()) return;

	for (auto collider : colliderList_)	collider->Inactivate();
}

//�q���̉��
void IGameObject::ReleaseAllChildren()
{
	//childList_�̉��
	for (auto child : childList_)
	{
		child->ReleaseSub();
		SAFE_DELETE(child);
	}
	childList_.clear();
}

//�q���̕`����s������ɍs���`�揈��
void IGameObject::DrawAfterChildren()
{
}

//�����������̏���
void IGameObject::OnCollision(IGameObject* pTarget, CollisionData *pData)
{
}

//�����蔻��̈ʒu��ݒ�
void IGameObject::SetColliderPosition(int hCollider, XMVECTOR const* posision)
{
	if (hCollider >= colliderList_.size())	return;	//�w��ԍ��̓����蔻�肪���݂��Ȃ���Ή������Ȃ�
	if (posision == nullptr)				return;	//�����z��|�C���^�̒l��nulptr�Ȃ牽�����Ȃ�

	//�����蔻��̈ʒu��ݒ�
	colliderList_[hCollider]->SetPosision(posision);
}

//�����蔻��̈ʒu��ݒ�
void IGameObject::SetColliderSize(int hCollider, XMVECTOR const& size)
{
	if (hCollider >= colliderList_.size())	return;	//�w��ԍ��̓����蔻�肪���݂��Ȃ���Ή������Ȃ�

	//�����蔻��̑傫����ݒ�
	colliderList_[hCollider]->SetSize(size);
}

//����V�[�����̓����蔻��
void IGameObject::Collision()
{
	//�����蔻��������ĂȂ���Ή������Ȃ�
	if (colliderList_.empty()) return;

	pParent_->CollisionSub(this);
}