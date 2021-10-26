#pragma once
#include "../Engine/IUserInterface.h"
#include "ResultScene.h"

//�^�C�g���o�b�N�{�^��
class TitleBackButton : public IUserInterface
{
private:

	ResultScene* pResultScene_;	//���U���g�V�[���̃|�C���^

public:

	TitleBackButton();
	TitleBackButton(ResultScene* pResultScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};