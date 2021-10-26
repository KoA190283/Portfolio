#pragma once
#include "../Engine/IUserInterface.h"
#include "ResultScene.h"

//�Q�[���I���{�^��
class EndGameButton : public IUserInterface
{
private:

	ResultScene* pResultScene_;	//���U���g�V�[���̃|�C���^

public:

	EndGameButton();
	EndGameButton(ResultScene* pResultScene);

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};