#pragma once
#include "IGameObject.h"

//GameObject���p������N���X�̃��[�g�ƂȂ�I�u�W�F�N�g
class RootJob :public IGameObject
{
public:

	RootJob();
	~RootJob();

	//������
	void Initialize()	override;

private:

	//�X�V
	void Update()		override;
	
	//�`��
	void Draw()			override;
	
	//���
	void Release()		override;
};

