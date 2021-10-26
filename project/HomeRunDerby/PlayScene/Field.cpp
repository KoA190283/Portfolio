#include "../Engine/BoxCollider.h"
#include "../Engine/Model.h"
#include "../ExternalDataManager.h"
#include "Field.h"

//�R���X�g���N�^
Field::Field(IGameObject* parent)
	: IGameObject(parent, "Field")
{
}

//������
void Field::Initialize()
{
	//���f���f�[�^�̃��[�h
	const int MODEL_HANDLE_FIELD_OBJECT = Model::Load(ExternalDataManager::GetFilePathFieldObjectModel());
	assert(MODEL_HANDLE_FIELD_OBJECT != Model::ERROR_CORD);

	const int MODEL_HANDLE_GROUND		= Model::Load(ExternalDataManager::GetFilePathGroundModel());
	assert(MODEL_HANDLE_GROUND != Model::ERROR_CORD);

	//�t�B�[���h�f�[�^��ݒ�
	ExternalDataManager::SetFieldData(DirectX::g_XMZero, DirectX::XMVectorSet(0, 0, -18.44f, 0), 100.f);

	//�T�C�Y�ύX
	transform_.scale_ = DirectX::XMVectorSet(400.f, 0.00000001f, 400.f, 0);

	//�����蔻��̐ݒ�ǉ�
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;												//�I�u�W�F�N�g���瓖���蔻��ւ̃x�N�g��
	BoxCollider*	newCollider					= new BoxCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//�V���������蔻��
	AddCollider(newCollider);

	//�X�V�͂��Ȃ�
	Inactivate();
}

//�X�V
void Field::Update()
{
}

//�`��
void Field::Draw()
{
	//���f���f�[�^�̃n���h���ԍ����擾
	const int MODEL_HANDLE_GROUND		= Model::GetHandle(ExternalDataManager::GetFilePathGroundModel());			//�O���E���h�̃��f���̃n���h��
	const int MODEL_HANDLE_FIELD_OBJECT = Model::GetHandle(ExternalDataManager::GetFilePathFieldObjectModel());	//�t�B�[���h�I�u�W�F�N�g�̃��f���̃n���h��

	//�n�ʂ̕`��
	Model::SetTransform(MODEL_HANDLE_GROUND, transform_);
	Model::Draw(MODEL_HANDLE_GROUND, Direct3D::SHADER_TYPE::GROUND);

	//�t�B�[���h�I�u�W�F�N�g�̕`��
	Transform trans;
	trans.position_.vecY += 0.0001f;								//������������
	Model::SetTransform(MODEL_HANDLE_FIELD_OBJECT, trans);
	Model::Draw(MODEL_HANDLE_FIELD_OBJECT,Direct3D::SHADER_TYPE::NO_SHADE_3D);
}

//���
void Field::Release()
{
}