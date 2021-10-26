#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"

//�^�C�g���V�[���̊Ǘ�
class TitleScene : public IGameObject
{
private:

	std::list<IUserInterface*> userInterfaceList_;	//UI�N���X�̃��X�g

public:

	//�R���X�g���N�^
	TitleScene(IGameObject* parent);

	//������
	void Initialize() override;

	//�V�[����ύX����
	void ChangeNextScene() const;

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;
};