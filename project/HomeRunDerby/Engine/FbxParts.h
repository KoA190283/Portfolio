#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>

#include "Fbx.h"
#include "Transform.h"

//FBXSDK���g�p����̂ɕK�v�ȃ��C�u����
#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")


//�O���錾
class  Texture;
struct RayCastData;

const int POLYGON_VERTICES = 3;	//�|���S�����Ƃ̒��_��

//FBX�̃f�[�^���p�[�c���ƂɊǗ�����N���X
class FbxParts
{
	using XMMATRIX		= DirectX::XMMATRIX;
	using XMVECTOR		= DirectX::XMVECTOR;
	using XMFLOAT4		= DirectX::XMFLOAT4;
	using XMFLOAT4X4	= DirectX::XMFLOAT4X4;

private:

	//�R���X�^���g�o�b�t�@�[
	struct CONSTANT_BUFFER
	{
		XMMATRIX vertexConversionMatrix;//���_�ϊ��p�s��
		XMMATRIX normalConversionMatrix;//�@���ϊ��p�s��
		XMMATRIX matWorld;				//���[���h�s��

		XMFLOAT4 diffuseColor;			//�}�e���A���̐F
		XMFLOAT4 ambient;				//����
		XMFLOAT4 speculer;				//���ʔ���

		XMFLOAT4 cameraPosition;		//�J�����̈ʒu
		FLOAT	 shininess;				//�P�x
		BOOL	 isTexture;				//�e�N�X�`���\���Ă��邩�ǂ���
	};

	//���_���
	struct VERTEX
	{
		XMVECTOR position;				//��ʓ��̒��_���W
		XMVECTOR uv;					//UV���W
		XMVECTOR normal;				//�@��
		XMVECTOR tangent;				//�ڐ�
	};

	//�}�e���A��
	struct MATERIAL
	{
		XMFLOAT4 diffuse;				//�g�U���ˌ�
		XMFLOAT4 ambient;				//����
		XMFLOAT4 specular;				//���ʔ��ˌ�
		float	 shininess;				//�P�x
		Texture* pDiffuseTexture;		//�e�N�X�`��
		Texture* pNormalTexture;		//�m�[�}���e�N�X�`��
	};

	//�{�[���\���́i�֐ߏ��j
	struct  Bone
	{
		XMMATRIX bindPose;				//�����|�[�Y���̃{�[���ϊ��s��
		XMMATRIX newPose;				//�A�j���[�V�����ŕω������Ƃ��̃{�[���ϊ��s��
		XMMATRIX diffPose;				//mBindPose �ɑ΂��� mNowPose �̕ω���
	};

	//�E�F�C�g�\���́i�{�[���ƒ��_�̊֘A�t���j
	struct Weight
	{
		XMVECTOR positionOrigin;		//���X�̒��_���W
		XMVECTOR normalOrigin;			//���X�̖@���x�N�g��
		int*	 pBoneIndex;			//�֘A����{�[����ID
		float*	 pBoneWeight;			//�{�[���̏d��
	};

	ID3D11Buffer*	pVertexBuffer_;			//���_�o�b�t�@
	ID3D11Buffer**	ppIndexBuffer_;			//�C���f�b�N�X�o�b�t�@
	ID3D11Buffer*	pConstantBuffer_;		//�R���X�^���g�o�b�t�@

	MATERIAL*		pMaterialDatas_;		//�}�e���A���̃��X�g
	VERTEX*			pVertexDatas_;			//���_���̃��X�g
	int**			ppIndexDatas_;			//�}�e���A�����̃C���f�b�N�X���̃��X�g

	DWORD			vertexCount_;			//���_��
	DWORD			polygonCount_;			//�|���S����
	DWORD			materialCount_;			//�}�e���A���̌�
	DWORD*			indexCountEachMaterial_;//�}�e���A�����̃C���f�b�N�X��

	FbxSkin*		pSkinInfo_;				//�X�L�����b�V�����i�X�L�����b�V���A�j���[�V�����̃f�[�^�{�́j
	FbxCluster**	ppCluster_;				//�N���X�^���i�֐߂��ƂɊ֘A�t����ꂽ���_���j
	Bone*			pBoneArray_;			//�e�֐߂̏��
	Weight*			pWeightArray_;			//�E�F�C�g���i���_�̑΂���e�֐߂̉e���x�����j
	int				numBone_;				//FBX�Ɋ܂܂�Ă���֐߂̐�

public:

	FbxParts();
	~FbxParts();

	//FBX�̃f�[�^���g�p���鏀��
	//�����FpNode			FBX�t�@�C���̃V�[��
	//�ߒl�F�����̐���
	HRESULT InitFbxPartsData(FbxNode* pNode);

	//�`��
	//�����Ftransform		�ϊ����
	void Draw(Transform & transform) const;

	//�A�j���[�V�����̎p���ɕω������邽�߂̍s����擾
	//�����P�FBoneNumber	�擾����{�[���̔ԍ�
	//�����Q�Ftime			�A�j���[�V�����̃t���[��
	FbxMatrix GetAnimationMatrix(int BoneNumber, const FbxTime& time);

	//�A�j���[�V�����ɍ��킹�Ďp����ݒ�
	//�����Ftime			�A�j���[�V�����̃t���[��
	void SetAnimationPose(const FbxTime& time);

	//��̎p���̍�����V�����p���Ƃ��Đݒ�
	//�����P�FpFbxParts		���������FbxParts
	//�����Q�Fpercentage	pFbxParts�̎p���ɂǂꂾ���񂹂邩�̔䗦
	void SetDifferencePose(FbxParts* pFbxParts, float percentage);

	//���FBX�̏����p�����猻�݂̎p���ւ̍�������������
	//�����P�FpFbxParts		��������FbxParts
	void AddDifferencePose(FbxParts * pFbxParts);

	//�ό`����
	void Deform();

	//�w�肵�����O�̃{�[���̈ʒu���擾
	//�����P�FboneName�@	�ʒu���擾����{�[���̖��O
	//�����Q�Fposition		�擾�����ʒu���i�[����A�h���X
	//�ߒl�@�F�����P�Ŏw�肵�����O�̃{�[�������݂�����
	bool GetBonePosition(const std::string& boneName, XMVECTOR* position) const;

	//���C�L���X�g
	//�����P�FpData�@		���C�L���X�g�Ɏg�p����f�[�^
	//�����Q�Ftransform		�ϊ����
	void RayCast(RayCastData* pData, Transform& transform) const;

	//���
	void Release();

private:

	//���_�o�b�t�@����
	//�����FpMesh		���b�V�����
	//�ߒl�F�����̐���
	HRESULT InitVertex(FbxMesh* pMesh);

	//���_�����N���X�ϐ��Ɋi�[
	//�����FpMesh		���b�V�����
	void SetVertexData(FbxMesh* pMesh);

	//���_�o�b�t�@�̍쐬
	//�ߒl�F�����̐���
	HRESULT CreateVertexBuffer();

	//�C���f�b�N�X�o�b�t�@����
	//�����FpMesh		���b�V�����
	//�ߒl�F�����̐���
	HRESULT InitIndex(FbxMesh* pMesh);

	//�C���f�b�N�X�����N���X�ϐ��Ɋi�[
	//�����P�FpMesh		���b�V�����
	//�����Q�Fmaterial	�擾����}�e���A���̗v�f�ԍ�
	void SetIndexData(FbxMesh* pMesh,int material);

	//�C���f�b�N�X�o�b�t�@�̍쐬
	//�����P�Fmaterial		�擾����}�e���A���̗v�f�ԍ�
	//�����Q�FindexCount	�擾����}�e���A���̃C���f�b�N�X��
	//�ߒl�@�F�����̐���
	HRESULT CreateIndexBuffer(int material, int indexCount);

	//�R���X�^���g�o�b�t�@����
	//�ߒl�F�����̐���
	HRESULT InitConstantBuffer();

	//�}�e���A������
	//�����FpNode	�m�[�h���
	//�ߒl�F�����̐���
	HRESULT InitMaterial(FbxNode* pNode);

	//�e�N�X�`���̓ǂݍ���
	//�����P�FpMaterial	�}�e���A���f�[�^
	//�����Q�Fmaterial	�擾����}�e���A���̗v�f�ԍ�
	//�ߒl�@�F�����̐���
	HRESULT InitDiffuseTexture(fbxsdk::FbxSurfaceMaterial * pMaterial, int material);

	//�@���}�b�v�e�N�X�`���̓ǂݍ���
	//�����P�FpMaterial	�}�e���A���f�[�^
	//�����Q�Fmaterial	�擾����}�e���A���̗v�f�ԍ�
	//�ߒl�@�F�����̐���
	HRESULT InitNormalTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material);

	
	//���̏�������
	//�����FpMesh		���b�V�����
	void InitSkelton(FbxMesh* pMesh);

	//�{�[���̃E�G�C�g�̍쐬
	void CreateBoneWeight();

	//�{�[���̉e���͂𒸓_���Ƃɐ������Ċi�[
	void SetBoneWeight();

	//�{�[���̍쐬
	void CreateBone();

	//�}�e���A�����Ƃɕ`��
	//�����Ftransform			�`�掞�̕ό`���
	void DrawMaterial(Transform& transform) const;
};