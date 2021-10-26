#include "BackGround.h"

#include "Engine/Image.h"
#include "ExternalDataManager.h"

//�w�i�`��
namespace BackGround
{
	//�ϐ�
	namespace
	{
		BACK_GROUND_TYPE currentBackGroundType_;		//���ݐݒ肳��Ă���w�i
	}

	//������
	void Initialize()
	{
		//�w�i�`��Ɏg�p����f�[�^��ǂݍ���
		ExternalDataManager::LoadBackGroundData("Data/BackGroundData.csv");

		//���f���f�[�^�̓ǂݍ���
		for (int i = 0; i < (int)BACK_GROUND_TYPE::MAX; i++) 
		{
			const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathBackGroundModel(i));
			assert(IMAGE_HANDLE != Image::ERROR_CORD);
		}

		//��w�i�������ݒ�ɂ���
		currentBackGroundType_ = BACK_GROUND_TYPE::SKY;
	}

	//�`��
	void Draw()
	{
		using DirectX::operator*=;

		//�`�悷�郂�f���̏�Ԃ�ݒ肵�ĕ`��
		const int					IMAGE_HANDLE	= Image::GetHandle(ExternalDataManager::GetFilePathBackGroundModel((int)currentBackGroundType_));	//���f���n���h��
		const Direct3D::SHADER_TYPE SHADER_TYPE		= (Direct3D::SHADER_TYPE)ExternalDataManager::GetShaderTypeBackGround((int)currentBackGroundType_);	//�g�p����V�F�[�_�[

		Transform transform;
		transform.position_.vecZ	+= 0.9999999f;		//Z�͐[�x�l����(0~1) 1�𒴂���ƕ`�悳��Ȃ�
		Image::SetTransform(IMAGE_HANDLE, transform);
		Image::Draw(IMAGE_HANDLE, SHADER_TYPE);
	}

	//�g�p����w�i��ύX����
	void ChangeBackGround(BACK_GROUND_TYPE type)
	{
		currentBackGroundType_ = type;
	}

	//���
	void Release()
	{
	}
}

