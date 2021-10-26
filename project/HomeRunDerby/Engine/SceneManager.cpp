#include "SceneManager.h"

#include "../PlayScene/PlayScene.h"
#include "../ResultScene/ResultScene.h"
#include "../TitleScene/TitleScene.h"
#include "Image.h"
#include "Model.h"

SceneManager::SceneManager(IGameObject* parent)
	: IGameObject(parent, "SceneManager"), currentSceneID_(SCENE_ID::TITLE), nextSceneID_(SCENE_ID::TITLE)
{
}

SceneManager::~SceneManager()
{
}

//������
void SceneManager::Initialize()
{
	//�^�C�g���V�[�����J�n��ʂ�
	currentSceneID_ = SCENE_ID::TITLE;
	nextSceneID_	= currentSceneID_;
	Instantiate<TitleScene>(this);
}

//�V�[���؂�ւ�
void SceneManager::ChangeScene(SCENE_ID nextSceneId)
{
	nextSceneID_ = nextSceneId;
}

//�X�V
void SceneManager::Update()
{
	//���݂̃V�[���Ǝ��̃V�[���������ł���΂Ȃɂ����Ȃ�
	if (currentSceneID_ == nextSceneID_) return;

	//���݂̃V�[�����I��点��
	Model::Release();		//���f���f�[�^�̉��
	Image::Release();		//�C���[�W�f�[�^�̉��
	ReleaseAllChildren();	//�V�[���I�u�W�F�N�g�ȉ��̑S���

	//���̃V�[�����J�n
	switch (nextSceneID_)
	{
	case SCENE_ID::TITLE : Instantiate<TitleScene>(this) ; break;	//�^�C�g���V�[��
	case SCENE_ID::PLAY  : Instantiate<PlayScene>(this)  ; break;	//�v���C�V�[��
	case SCENE_ID::RESULT: Instantiate<ResultScene>(this); break;	//���U���g�V�[��
	}

	//���݂̃V�[�������̃V�[���ɂ���
	currentSceneID_ = nextSceneID_;
}

//�`��
void SceneManager::Draw()
{

}

//���
void SceneManager::Release()
{
}

