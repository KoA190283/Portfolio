#pragma once
#include "Transform.h"
#include <string>

//UI�̒��ۃN���X
class IUserInterface
{
protected:

	Transform transform_;				//�ό`���

public:

	IUserInterface();
	virtual ~IUserInterface();

	virtual void Initialize()	= 0;	//������
	virtual void Update()		= 0;	//�X�V
	virtual void Draw()			= 0;	//�`��
	virtual void Release()		= 0;	//���
};

