#include "RootJob.h"

#include "SceneManager.h"

RootJob::RootJob()
{
}

RootJob::~RootJob()
{
}

//������
void RootJob::Initialize()
{
	//�V�[���}�l�[�W���[�̐���
	Instantiate<SceneManager>(this);
}

//�X�V
void RootJob::Update()
{
}

//�`��
void RootJob::Draw()
{
}

//���
void RootJob::Release()
{
}
