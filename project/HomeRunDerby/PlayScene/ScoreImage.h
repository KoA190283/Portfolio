#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//�X�R�A�摜
class ScoreImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//�v���C�V�[���̃|�C���^

public:

	ScoreImage();
	ScoreImage(PlayScene* pPlayScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};