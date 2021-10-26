#pragma once
#include "../Engine/IGameObject.h"

//�t�B�[���h�̊Ǘ�
class Field : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//�R���X�g���N�^
	Field(IGameObject* parent);

	//������
	void Initialize() override;

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};