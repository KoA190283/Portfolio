#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//�ړI�摜
class PurposeImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//�v���C�V�[���̃|�C���^

public:

	PurposeImage();
	PurposeImage(PlayScene* pPlayScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};