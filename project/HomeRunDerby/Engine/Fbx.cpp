#include "Fbx.h"

#include "Direct3D.h"
#include "Camera.h"
#include "Math.h"
#include "Global.h"

using DirectX::XMVECTOR;

//������
Fbx::Fbx() :
	pFbxManager_(nullptr),
	pFbxScene_(nullptr),
	frameRate_(FbxTime::EMode::eDefaultMode)
{
};

//�f�X�g���N�^
Fbx::~Fbx()
{
	Release();
};

//FBX�f�[�^�̃��[�h
HRESULT Fbx::Load(std::string const & fileName)
{
	//FbxScene�Ƀf�[�^������
	if (FAILED(InitFbxScene(fileName)))
	{
		return E_FAIL;
	}

	//FBX�̃f�[�^���N���X�Ɋi�[
	if (FAILED(InitFbxParts(fileName)))
	{
		return E_FAIL;
	}
	
	return S_OK;
};

//�`��
void Fbx::Draw(Transform & transform) const
{	
	//�p�[�c���Ƃɕ`��
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->Draw(transform);
	}
};

//�A�j���[�V�����ɍ��킹�ĕό`����
void Fbx::DeformAnimationPose(int frame)
{
	//�A�j���[�V�����ɍ��킹�ă{�[����ݒ�
	SetAnimationPose(frame);

	//�ό`����
	Deform();
}

//�A�j���[�V�����ɍ��킹�ă{�[����ݒ�
void Fbx::SetAnimationPose(int frame)
{
	//FbxTime��frame�ɐݒ肷��
	FbxTime fbxTime;
	fbxTime.SetTime(0, 0, 0, frame, 0, 0, frameRate_);

	//�p�[�c���Ƃɐݒ�
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->SetAnimationPose(fbxTime);
	}
}

//���FBX�̎p���̍�����V�����p���Ƃ��Đݒ�
void Fbx::SetDifferencePose(Fbx * pFbx, float percentage)
{
	if (pFbx == nullptr)	return;				//�����|�C���^��nullptr�ł���Ώ����s��

	//�p�[�c���Ƃɐݒ�
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->SetDifferencePose(pFbx->fbxPartsList_[i], percentage);
	}
}

//���FBX�̏����p�����猻�݂̎p���ւ̍�������������
void Fbx::AddDifferencePose(Fbx * pFbx)
{
	if (pFbx == nullptr)	return;				//�����|�C���^��nullptr�ł���Ώ����s��

	//�p�[�c���Ƃɐݒ�
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->AddDifferencePose(pFbx->fbxPartsList_[i]);
	}
}

//�ό`����
void Fbx::Deform()
{
	//�p�[�c���Ƃɐݒ�
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->Deform();
	}
}

//�{�[���̈ʒu���擾
XMVECTOR Fbx::GetBonePosition(const std::string& boneName) const
{
	XMVECTOR bonePosision = DirectX::g_XMZero;

	//�p�[�c���Ƃɖ��O�̈�v����{�[����T���A��������̈ʒu��
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		//�{�[���̈ʒu���擾�o�����烋�[�v�𔲂���
		if (fbxPartsList_[i]->GetBonePosition(boneName, &bonePosision))	break;
	}

	return bonePosision;
}

//���C�L���X�g
void Fbx::RayCast(RayCastData* pData, Transform& transform) const
{
	if (pData == nullptr)	return;				//�����|�C���^��nullptr�ł���Ώ����s��

	pData->isHit = false;

	//�p�[�c���ƂɃ��C�L���X�g
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->RayCast(pData, transform);

		//�Փ˂��Ă�����I��
		if (pData->isHit)	return;
	}
}

//���
void Fbx::Release()
{
	pFbxScene_->Destroy();
	pFbxManager_->Destroy();
};

//FbxScene�Ƀf�[�^������
HRESULT Fbx::InitFbxScene(const std::string& fileName)
{
	FbxImporter* pFbxImporter = nullptr;	//Fbx�t�@�C���̎��f�[�^�����b�V����J�����Ȃǂ̏��ɕ���������

	//�}�l�[�W���𐶐�
	if ((pFbxManager_ = FbxManager::Create()) == nullptr)
	{
		MessageBox(nullptr, "FBX�}�l�[�W���[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�C���|�[�^�[�𐶐�
	if ((pFbxImporter = FbxImporter::Create(pFbxManager_, "imp")) == nullptr)
	{
		MessageBox(nullptr, "FBX�C���|�[�^�[�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�w�肵��FBX�t�@�C���ŏ���������
	if (pFbxImporter->Initialize(fileName.c_str(), -1, pFbxManager_->GetIOSettings()) == false)
	{
		MessageBox(nullptr, "�t�@�C���̏������Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//FBX�V�[���̍쐬
	if ((pFbxScene_ = FbxScene::Create(pFbxManager_, "fbxscene")) == nullptr)
	{
		MessageBox(nullptr, "FBX�V�[���̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	//�V�[���I�u�W�F�N�g��FBX�t�@�C���̏��𗬂�����
	if (pFbxImporter->Import(pFbxScene_) == false)
	{
		MessageBox(nullptr, "�f�[�^�̃C���|�[�g�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	pFbxImporter->Destroy();

	return S_OK;
}

//FBX�̃f�[�^���g�p���鏀��
HRESULT Fbx::InitFbxParts(const std::string& fileName)
{
	//���݂̃J�����g�f�B���N�g�����o���Ă���
	char defaultCurrentDirectory[MAX_PATH];				//�J�����g�f�B���N�g���̃p�X
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	char fileDirectory[_MAX_PATH];						//FBX�t�@�C��������f�B���N�g���i�t�H���_�j��

	//�p�X�̎擾
	_splitpath_s(fileName.c_str(), nullptr, 0, fileDirectory, _MAX_PATH, nullptr, 0, nullptr, 0);

	//�J�����g�f�B���N�g���ύX
	SetCurrentDirectory(fileDirectory);

	//���[�g�m�[�h�̎擾
	FbxNode* rootNode = pFbxScene_->GetRootNode();

	//�q�̐������m�[�h�𒲂ׂ�
	const int CHILD_COUNT = rootNode->GetChildCount();
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		CheckNode(rootNode->GetChild(i));
	}

	//�t���[�����[�g�̎擾
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	//�J�����g�f�B���N�g�������ɖ߂�
	SetCurrentDirectory(defaultCurrentDirectory);

	return S_OK;
}

//�m�[�h�̒��Ƀ��b�V����񂪂��邩���m�F���A����΃p�[�c���g�p�\�ȏ�Ԃɂ���
void Fbx::CheckNode(FbxNode * pNode)
{
	//���̃m�[�h�ɂ̓��b�V����񂪓����Ă��邾�낤���H
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//�p�[�c��p��
		FbxParts* pParts = new FbxParts;
		pParts->InitFbxPartsData(pNode);

		//�p�[�c���𓮓I�z��ɒǉ�
		fbxPartsList_.push_back(pParts);
	}

	//�q�m�[�h�ɂ��f�[�^���m�F
	int childCount = pNode->GetChildCount();

	//��l���`�F�b�N
	for (int i = 0; i < childCount; i++)
	{
		CheckNode(pNode->GetChild(i));
	}
}