#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//�z�[���������摜
class HomeRunImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//�v���C�V�[���̃|�C���^

public:

	HomeRunImage();
	HomeRunImage(PlayScene* pPlayScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};