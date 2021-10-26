#pragma once
#include "../Engine/IGameObject.h"

//�{�[���̊Ǘ�
class StrikeZone : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

public:

	//�R���X�g���N�^
	StrikeZone(IGameObject* parent);

	//������
	void Initialize() override;

	//���I�u�W�F�N�g�ƏՓ˂������̏���
	//�����P�FpTarget		���������I�u�W�F�N�g
	//�����Q�FpData			�Փˎ��̃f�[�^
	void OnCollision(IGameObject* pTarget, CollisionData *pData) override;

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;

	//�����ꂽ�i�N���b�N���ꂽ�j�Ƃ��̏���
	void Pressed();

	//�I�ɂȂ�{�[���̕`��
	void DrawTagetBall();
};