#include "Model.h"

#include "Camera.h"
#include "Global.h"

//���f���f�[�^�̊Ǘ�
namespace Model 
{
	namespace
	{
		//���f���f�[�^
		struct ModelData
		{
			std::string fileName;		//�t�@�C�����i�p�X�j
			Transform	transform;		//��ԁi�ʒu�A�g��k���Ȃǁj
			Fbx*		pFbx;			//���f���f�[�^

			//�R���X�g���N�^
			ModelData() :pFbx(nullptr) {}
		};

		std::vector<ModelData*> datas_;	//���f���f�[�^�̏W����
	}

	//���f���f�[�^�̓ǂݍ���
	int Load(const std::string& fileName)
	{
		//���Ƀ��[�h�ς݂ł���΂��̃n���h���ԍ���Ԃ�
		const int HANDLE = GetHandle(fileName);
		if (HANDLE != ERROR_CORD)	return HANDLE;

		//FBX�f�[�^�̃��[�h
		ModelData* pData = new ModelData;
		pData->pFbx		 = new Fbx;
		if (FAILED(pData->pFbx->Load(fileName)))
		{
			SAFE_DELETE(pData->pFbx);
			SAFE_DELETE(pData);
			return ERROR_CORD;
		}

		//�t�@�C������ݒ�
		pData->fileName = fileName;

		//���f���f�[�^���i�[
		datas_.push_back(pData);

		//�n���h����Ԃ�
		return (int)datas_.size() - 1;
	}

	//�\�������Ԃ�ݒ�
	void SetTransform(int handle, Transform& transform)
	{
		datas_[handle]->transform = transform;
	}

	//�`��
	void Draw(int handle)
	{
		//�g�p����V�F�[�_�[�̎w�肪�Ȃ����DEFAULT_3D�ŕ`��
		Draw(handle, Direct3D::SHADER_TYPE::DEFAULT_3D);
	}

	//�`��
	void Draw(int handle, Direct3D::SHADER_TYPE type)
	{
		//�g�p����V�F�[�_�[�̃Z�b�g���w�肷��
		Direct3D::SetShaderBundle(type);

		//�ϊ��s����Čv�Z
		datas_[handle]->transform.Calclation();

		//�`��
		datas_[handle]->pFbx->Draw(datas_[handle]->transform);
	}

	//�w�肳�ꂽ���Ԃ̃A�j���[�V�����̏�Ԃɕό`����
	void DeformAnimationPose(int handle, int frame)
	{
		datas_[handle]->pFbx->DeformAnimationPose(frame);
	}

	//�w�肳�ꂽ���Ԃ̃A�j���[�V�����̎p������ݒ肷��i�ό`�͂��Ȃ��j
	void SetAnimationPose(int handle, int frame)
	{
		datas_[handle]->pFbx->SetAnimationPose(frame);
	}

	//��̃��f���̎p���̍�����V�����p���Ƃ��Đݒ�i�ό`�͂��Ȃ��j
	void SetDifferencePose(int Basehandle, int materialHandle, float percentage)
	{
		datas_[Basehandle]->pFbx->SetDifferencePose(datas_[materialHandle]->pFbx, percentage);
	}

	//��̃��f���̏����p�����猻�݂̎p���ւ̍������������V�����p���Ƃ��Đݒ�i�ό`�͂��Ȃ��j
	void AddDifferencePose(int Basehandle, int materialHandle)
	{
		datas_[Basehandle]->pFbx->AddDifferencePose(datas_[materialHandle]->pFbx);
	}

	//�ݒ肳��Ă���p�����ŕό`����
	void Deform(int handle)
	{
		datas_[handle]->pFbx->Deform();
	}

	//�{�[���̈ʒu���擾����
	XMVECTOR GetBonePosition(int handle, const std::string& boneName)
	{
		return datas_[handle]->pFbx->GetBonePosition(boneName);
	}

	//���[�h�ς݃��f���̃n���h���ԍ����擾����
	int GetHandle(const std::string& fileName)
	{
		//���[�h�ς݂̃��f���f�[�^����t�@�C��������v������̂�T���ăn���h���ԍ���Ԃ�
		for (int i = 0; i < datas_.size(); i++)
			if (datas_[i]->fileName == fileName) return i;

		//���݂��Ȃ����-1��Ԃ�
		return ERROR_CORD;
	}

	//���
	void Release()
	{
		auto it = datas_.begin();
		while (it != datas_.end())
		{
			SAFE_DELETE((*it)->pFbx);
			SAFE_DELETE((*it));
			it++;
		}
		datas_.clear();
	}

	//���C�L���X�g
	void RayCast(int handle, RayCastData* rayData)
	{
		using DirectX::operator+;
		using DirectX::operator-;

		//���C�̃f�[�^�����[���h���W���烍�[�J�����W�ɕϊ�
		const XMMATRIX INVERSE_WORLD_MATRIX	= XMMatrixInverse(nullptr, datas_[handle]->transform.GetWorldMatrix());	//���[���h�s��̋t�s��
		XMVECTOR passingPoint				= rayData->start + rayData->direction;									//���C�̒ʉߓ_
		passingPoint						= XMVector3TransformCoord(passingPoint, INVERSE_WORLD_MATRIX);			//���C�ʉߓ_��invWorld�ŕϊ������[�J�����W��

		rayData->start		= XMVector3TransformCoord(rayData->start, INVERSE_WORLD_MATRIX);						//���C�n�_��invWorld�ŕϊ������[�J�����W��
		rayData->direction	= DirectX::XMVector3Normalize(passingPoint - rayData->start);							//�n�_->�ʉߓ_�̃x�N�g�������C�̌���

		//���C�L���X�g
		datas_[handle]->pFbx->RayCast(rayData, datas_[handle]->transform);
	}
}