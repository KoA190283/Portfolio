#pragma once
#include "../Engine/IUserInterface.h"
#include "TitleScene.h"

//���ʁ@���[�h�I���{�^��
class NormalButton : public IUserInterface
{
private:

	TitleScene* pTitleScene_;	//�^�C�g���V�[���̃|�C���^

public:

	NormalButton();
	NormalButton(TitleScene* pTitleScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};