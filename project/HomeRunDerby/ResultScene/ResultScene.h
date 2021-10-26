#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"

//���U���g�V�[���̊Ǘ�
class ResultScene : public IGameObject
{
private:

	std::list<IUserInterface*>	userInterfaceList_;	//UI�N���X�̃��X�g

public:

	//�R���X�g���N�^
	ResultScene(IGameObject* parent);

	//������
	void Initialize() override;

	//�V�[����ύX����
	void ChangeSceneTitle() const;

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};