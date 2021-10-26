#pragma once
#include "../Engine/IUserInterface.h"
#include "TitleScene.h"

//�Ղ����@���[�h�I���{�^��
class EasyButton : public IUserInterface
{
private:

	TitleScene* pTitleScene_;	//�^�C�g���V�[���̃|�C���^

public:

	EasyButton();
	EasyButton(TitleScene* pTitleScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};

