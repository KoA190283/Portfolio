#pragma once
#include "../Engine/IUserInterface.h"
#include "TitleScene.h"

//����@���[�h�I���{�^��
class HardButton : public IUserInterface
{
private:

	TitleScene* pTitleScene_;	//�^�C�g���V�[���̃|�C���^

public:

	HardButton();
	HardButton(TitleScene* pTitleScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};
