#pragma once
#include "../Engine/IUserInterface.h"

//�^�C�g���摜
class TitleImage : public IUserInterface
{
public:

	TitleImage();

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};