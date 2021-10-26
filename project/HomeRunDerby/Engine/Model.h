#pragma once
#include<string>
#include<vector>

#include"Direct3D.h"
#include"Fbx.h"
#include"Transform.h"

//���f���f�[�^�̊Ǘ�
namespace Model
{
	using DirectX::XMMATRIX;
	using DirectX::XMVECTOR;

	const int ERROR_CORD = -1;	//�摜�f�[�^�̎擾�Ɏ��s�����ꍇ�̔ԍ�

	//���f���f�[�^�̓ǂݍ���
	//�����FfileName�@	�t�@�C���̃p�X
	//�ߒl�F�n���h���ԍ�
	int Load(const std::string& fileName);

	//�\�������Ԃ�ݒ�
	//�����P�Fhandle		�n���h���ԍ�
	//�����Q�Ftransform		�\��������
	void SetTransform(int handle, Transform& transform);

	//�`��
	//�����Fhandle�@		�n���h���ԍ�
	void Draw(int handle);

	//�`��
	//�����P�Fhandle�@		�n���h���ԍ�
	//�����Q�Ftype�@		�`��Ɏg�p����V�F�[�_�[�̎��
	void Draw(int handle, Direct3D::SHADER_TYPE type);

	//�w�肳�ꂽ���Ԃ̃A�j���[�V�����̏�Ԃɕό`����
	//�����P�Fhandle�@		�n���h���ԍ�
	//�����Q�Fframe�@		�A�j���[�V�����̃t���[����
	void DeformAnimationPose(int handle, int frame);

	//�w�肳�ꂽ���Ԃ̃A�j���[�V�����̎p������ݒ肷��i�ό`�͂��Ȃ��j
	//�����P�Fhandle�@		�n���h���ԍ�
	//�����Q�Fframe�@		�A�j���[�V�����̃t���[����
	void SetAnimationPose(int handle, int frame);

	//��̎p���̍�����V�����p���Ƃ��Đݒ�i�ό`�͂��Ȃ��j
	//�����P�FBasehandle�@		�V�����p����ݒ肷��FBX�̃n���h���ԍ�
	//�����Q�FmaterialHandle�@	�V�����p���̐ݒ�ɉe����^����FBX�̃n���h���ԍ�
	//�����R�Fpercentage�@		materialHandle��FBX�̉e����
	void SetDifferencePose(int Basehandle, int materialHandle, float percentage);

	//��̃��f���̏����p�����猻�݂̎p���ւ̍������������V�����p���Ƃ��Đݒ�i�ό`�͂��Ȃ��j
	//�����P�FBasehandle�@		�V�����p����ݒ肷��FBX�̃n���h���ԍ�
	//�����Q�FmaterialHandle�@	�V�����p���̐ݒ�ɉe����^����FBX�̃n���h���ԍ�
	void AddDifferencePose(int Basehandle, int materialHandle);

	//�ݒ肳��Ă���p�����ŕό`����
	//�����Fhandle�@		�n���h���ԍ�
	void Deform(int handle);

	//�{�[���̈ʒu���擾����
	//�����P�Fhandle�@		�n���h���ԍ�
	//�����Q�FboneName�@	�{�[���̖��O
	//�ߒl�@�F�{�[���̈ʒu
	XMVECTOR GetBonePosition(int handle, const std::string& boneName);

	//���[�h�ς݃��f���̃n���h���ԍ����擾����
	//�����FfileName�@	�t�@�C���̃p�X
	//�ߒl�F�n���h���ԍ�
	int GetHandle(const std::string& fileName);

	//���
	void Release();

	//���C�L���X�g
	//�����P�Fhandle		�n���h���ԍ�
	//�����Q�FrayData		���C�L���X�g�Ɏg�p����f�[�^
	void RayCast(int handle, RayCastData* rayData);
};

