#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//�{�[�h�摜
class BordImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//�v���C�V�[���̃|�C���^

public:

	BordImage();
	BordImage(PlayScene* pPlayScene_);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};