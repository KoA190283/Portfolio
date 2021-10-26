#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>

#include "FbxParts.h"
#include "Transform.h"

//FBXSDK���g�p����̂ɕK�v�ȃ��C�u����
#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")

//���C�L���X�g�p�\����
struct RayCastData
{
	using XMVECTOR = DirectX::XMVECTOR;

	XMVECTOR	start;			//���C���ˈʒu
	XMVECTOR	direction;		//���C�̌����x�N�g��
	float		distance;		//�Փ˓_�܂ł̋���
	bool        isHit;			//���C������������
};

//�O���錾
class FbxParts;

//FBX���Ǘ�����N���X
class Fbx
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	FbxManager*				pFbxManager_;			//FBX�t�@�C���������@�\�̖{��
	FbxScene*				pFbxScene_;				//FBX�t�@�C���̃V�[��

	std::vector<FbxParts*>	fbxPartsList_;			//FBX�̃p�[�c���Ƃ̃f�[�^
	FbxTime::EMode			frameRate_;				//�A�j���[�V�����̃t���[�����[�g

public:

	Fbx();
	~Fbx();

	//FBX�f�[�^�̃��[�h
	//�����FfileName		���[�h����FBX�t�@�C���̖��O
	//�ߒl�F�����̐���
	HRESULT Load(std::string const & fileName);

	//�`��
	//�����Ftransform		�ϊ����
	void Draw(Transform & transform) const;

	//�A�j���[�V�����ɍ��킹�ĕό`����
	//�����Fframe			�A�j���[�V�����̃t���[��
	void DeformAnimationPose(int frame);
	
	//�A�j���[�V�����ɍ��킹�Ďp����ݒ�i�ό`�͂��Ȃ��j
	//�����Fframe			�A�j���[�V�����̃t���[��
	void SetAnimationPose(int frame);

	//���FBX�̎p���̍�����V�����p���Ƃ��Đݒ�i�ό`�͂��Ȃ��j
	//�����P�FpFbx			�A�j���[�V��������������FBX
	//�����Q�Fpercentage	��������䗦
	void SetDifferencePose(Fbx* pFbx, float percentage);

	//���FBX�̏����p�����猻�݂̎p���ւ̍�������������
	//�����FpFbx			�A�j���[�V��������������FBX
	void AddDifferencePose(Fbx* pFbx);

	//�ό`����
	void Deform();

	//�{�[���̈ʒu���擾
	//�����FboneName		�ʒu���擾����{�[���̖��O
	//�ߒl�F�{�[���̈ʒu
	XMVECTOR GetBonePosition(const std::string& boneName) const;

	//���C�L���X�g
	//�����Fdata�@			���C�L���X�g�Ɏg�p����f�[�^
	//�����Ftransform		�ϊ����
	void RayCast(RayCastData * pData, Transform& transform) const;

	//���
	void Release();

private:

	//FbxScene�Ƀf�[�^������
	//�����FfileName		���[�h����FBX�t�@�C���̖��O
	//�ߒl�F�����̐���
	HRESULT InitFbxScene(const std::string& fileName);

	//FBX�̃f�[�^���g�p���鏀��
	//�����FfileName		���[�h����FBX�t�@�C���̖��O
	//�ߒl�F�����̐���
	HRESULT InitFbxParts(const std::string& fileName);

	//�m�[�h�̒��Ƀ��b�V����񂪂��邩���m�F���A����΃p�[�c���g�p�\�ȏ�Ԃɂ���
	//�����FpNode	�m�[�h���
	void CheckNode(FbxNode* pNode);
};