#pragma once
#include "../Engine/IUserInterface.h"

//���ʔ��\�{�[�h�摜
class ResultBoardImage : public IUserInterface
{
public:

	ResultBoardImage();

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};
