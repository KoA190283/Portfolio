#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//�c��񐔉摜
class RemainingCountImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//�v���C�V�[���̃|�C���^

public:

	RemainingCountImage();
	RemainingCountImage(PlayScene* pPlayScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};