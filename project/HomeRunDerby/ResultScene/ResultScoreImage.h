#pragma once
#include "../Engine/IUserInterface.h"

//���ʉ摜
class ResultScoreImage : public IUserInterface
{
public:

	ResultScoreImage();

	//������
	void Initialize() override;

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};
