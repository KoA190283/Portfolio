#pragma once

//�w�i�`��
namespace BackGround
{
	enum class BACK_GROUND_TYPE
	{
		SKY = 0,
		MAX,
	};

	//������
	void Initialize();

	//�`��
	void Draw();

	//�g�p����w�i��ύX����
	void ChangeBackGround(BACK_GROUND_TYPE type);

	//���
	void Release();
}