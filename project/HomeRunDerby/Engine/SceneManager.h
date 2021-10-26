#pragma once
#include "IGameObject.h"

//�V�[����ID
enum class SCENE_ID
{
	TITLE = 0,		//�^�C�g���V�[��
	PLAY,			//�v���C�V�[��
	RESULT,		//���U���g�V�[��
	MAX,
};

//�V�[���̑J�ڂ��Ǘ�����N���X
class SceneManager :public IGameObject
{
private:

	SCENE_ID currentSceneID_;	//���݂̃V�[��ID
	SCENE_ID nextSceneID_;		//���̃V�[��ID

public:

	SceneManager(IGameObject* parent);
	~SceneManager();

	//������
	void Initialize() override;

	//�V�[���؂�ւ�
	//����:nextSceneId		���̃V�[����ID
	void ChangeScene(SCENE_ID nextSceneId);

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;

};

