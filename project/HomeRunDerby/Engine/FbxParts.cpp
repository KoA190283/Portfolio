#include "FbxParts.h"

#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"
#include "Math.h"
#include "Global.h"

//������
FbxParts::FbxParts() :

	pVertexBuffer_(nullptr),
	ppIndexBuffer_(nullptr),
	pConstantBuffer_(nullptr),

	pMaterialDatas_(nullptr),
	pVertexDatas_(nullptr),
	ppIndexDatas_(nullptr),

	vertexCount_(0),
	polygonCount_(0),
	materialCount_(0),
	indexCountEachMaterial_(nullptr),

	pSkinInfo_(nullptr),
	ppCluster_(nullptr),
	pBoneArray_(nullptr),
	pWeightArray_(nullptr),
	numBone_(0)
{
}; 

//�f�X�g���N�^
FbxParts::~FbxParts()
{
	Release();
};

//FBX�̃f�[�^���g�p���鏀��
HRESULT FbxParts::InitFbxPartsData(FbxNode* pNode)
{
	if (pNode == nullptr)	return E_FAIL;					//�����|�C���^��nullptr�ł���Ώ����s��

	//���b�V�������擾
	FbxMesh* pMesh		= pNode->GetMesh();					//���b�V�����

	//�e���̌����擾
	vertexCount_		= pMesh->GetControlPointsCount();	//���_�̐�
	polygonCount_		= pMesh->GetPolygonCount();			//�|���S���̐�
	materialCount_		= pNode->GetMaterialCount();		//�}�e���A����

	//���_�o�b�t�@����
	if (FAILED(InitVertex(pMesh))) return E_FAIL;

	//�C���f�b�N�X�o�b�t�@����
	if (FAILED(InitIndex(pMesh))) return E_FAIL;

	//�R���X�^���g�o�b�t�@����
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//�}�e���A������
	if (FAILED(InitMaterial(pNode))) return E_FAIL;

	//�X�P���g���̏���
	InitSkelton(pMesh);

	return S_OK;
}

//�`��
void FbxParts::Draw(Transform & transform) const
{
	//���_�o�b�t�@���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//�R���X�^���g�o�b�t�@���Z�b�g
	Direct3D::GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//���_�V�F�[�_�[�p
	Direct3D::GetContext()->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//�s�N�Z���V�F�[�_�[�p

	//�`��
	DrawMaterial(transform);
};

//�A�j���[�V�����̎p���ɕω������邽�߂̍s����擾
FbxMatrix FbxParts::GetAnimationMatrix(int const BoneNumber, const FbxTime& time)
{
	//����FBX��BoneNumber�ڂ̃{�[����time�̎��̏����擾����
	FbxAnimEvaluator* pEvaluator = ppCluster_[BoneNumber]->GetLink()->GetScene()->GetAnimationEvaluator();
	return pEvaluator->GetNodeGlobalTransform(ppCluster_[BoneNumber]->GetLink(), time);
}

//�A�j���[�V�����ɍ��킹�Ďp����ݒ�
void FbxParts::SetAnimationPose(const FbxTime& time)
{
	//�{�[�����Ƃ̌��݂̍s����擾����
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//time�̎��̃{�[���̕ό`�s����擾����
		FbxMatrix currentPose = GetAnimationMatrix(bone_i, time);

		// �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
		XMFLOAT4X4 pose;
		for (DWORD y = 0; y < 4; y++) {
			for (DWORD x = 0; x < 4; x++)
			{
				pose(x, y) = (float)currentPose.Get(x, y);
			}

			//�E��n(Maya)���獶��n(DirectX)�֕ϊ�
			pose.m[y][0] *= -1.f;
		}

		// �I�t�Z�b�g���̃|�[�Y�̍������v�Z���ă{�[���̃f�[�^��ύX����
		pBoneArray_[bone_i].newPose	 =  XMLoadFloat4x4(&pose);
		pBoneArray_[bone_i].diffPose =  XMMatrixInverse(nullptr, pBoneArray_[bone_i].bindPose);
		pBoneArray_[bone_i].diffPose *= pBoneArray_[bone_i].newPose;
	}
}

//��̎p���̍�����V�����p���Ƃ��Đݒ�
void FbxParts::SetDifferencePose(FbxParts * pFbxParts, float percentage)
{
	if (pFbxParts == nullptr)	return;															//�����|�C���^��nullptr�ł���Ώ����s��

	//�{�[�����Ƃɓ�̎p���̍�����V�����p���Ƃ��Đݒ�
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//��̎p���̍������擾��percentage���|���č����̑傫���𒲐�����
		const XMMATRIX DIFF_COMPOSITION_POSE = (pBoneArray_[bone_i].diffPose - pFbxParts->pBoneArray_[bone_i].diffPose) * percentage;	//���݂̎p������pFbxParts�̎p���ւ�percentage���̍�

		//�I�t�Z�b�g���̃|�[�Y����̍�����DIFF_COMPOSITION_POSE�Ƃ��ă{�[���̃f�[�^��ύX����
		pBoneArray_[bone_i].newPose  = pBoneArray_[bone_i].bindPose + DIFF_COMPOSITION_POSE;	//�V�����p��;	
		pBoneArray_[bone_i].diffPose = DIFF_COMPOSITION_POSE;									//�����p���Ƃ̍���
	}
}

//���݂̎p����pFbxParts��diffPose�����Z��������
void FbxParts::AddDifferencePose(FbxParts * pFbxParts)
{
	if (pFbxParts == nullptr)	return;												//�����|�C���^��nullptr�ł���Ώ����s��

	//�{�[�����ƂɌ��݂̎p����percentage������pFbxParts�̎p���ɋ߂Â���
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//�I�t�Z�b�g���̃|�[�Y�̍������v�Z���ă{�[���̃f�[�^��ύX����
		pBoneArray_[bone_i].diffPose += pFbxParts->pBoneArray_[bone_i].diffPose;	//�����p���Ƃ̍���
		pBoneArray_[bone_i].newPose	 += pFbxParts->pBoneArray_[bone_i].diffPose;	//�V�����p��;	
	}
}

//�ό`����
void FbxParts::Deform()
{
	//�e�{�[���ɑΉ��������_�̕ό`����
	for (DWORD vertex_i = 0; vertex_i < vertexCount_; vertex_i++)
	{
		//�e���_���Ƃɉe������{�[���̃E�F�C�g�l�𔽉f�������֐ߍs����쐬����
		XMMATRIX  matrix;
		ZeroMemory(&matrix, sizeof(matrix));
		for (int bone_i = 0; bone_i < numBone_; bone_i++)
		{
			if (pWeightArray_[vertex_i].pBoneIndex[bone_i] < 0)	break;																	//���_�ɃE�F�C�g�����{�[�����Ȃ��Ȃ�����s��쐬���[�v�𔲂���															

			matrix += pBoneArray_[pWeightArray_[vertex_i].pBoneIndex[bone_i]].diffPose * pWeightArray_[vertex_i].pBoneWeight[bone_i];	//�֐ߍs��Ƀ{�[���̃E�F�C�g�𔽉f������
		}

		//�쐬���ꂽ�֐ߍs����g���āA���_��ό`����
		const XMVECTOR POSITION				= pWeightArray_[vertex_i].positionOrigin;		//�ό`�O�̒��_���W
		const XMVECTOR NORMAL				= pWeightArray_[vertex_i].normalOrigin;			//�ό`�O�̖@��
		pVertexDatas_[vertex_i].position	= XMVector3TransformCoord(POSITION, matrix);	//���_�ʒu���֐ߍs��ŕϊ�
		pVertexDatas_[vertex_i].normal		= XMVector3TransformCoord(NORMAL, matrix);		//�@�����֐ߍs��ŕϊ�
	}

	//���_�o�b�t�@�����b�N���āA�ό`��������̒��_���ŏ㏑������
	D3D11_MAPPED_SUBRESOURCE msr		= {};						//�T�u���\�[�X�f�[�^�ւ̃A�N�Z�X
	ID3D11DeviceContext*	 pContext	= Direct3D::GetContext();	//�f�o�C�X�R���e�L�X�g�̃|�C���^

	pContext->Map(pVertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy_s(msr.pData, msr.RowPitch, pVertexDatas_, sizeof(VERTEX) * vertexCount_);
	pContext->Unmap(pVertexBuffer_, 0);
}

//�{�[���̈ʒu���擾
bool FbxParts::GetBonePosition(const std::string& boneName, XMVECTOR * position) const
{
	if (position == nullptr)	return false;				//�����|�C���^��nullptr�ł���Ώ������s��Ȃ�

	//���O��boneName�̃{�[����T���Ĉʒu���擾����
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//���O����v�����猻�݂̎p���ł̃{�[���̈ʒu�̈ʒu���擾����
		if (boneName == ppCluster_[bone_i]->GetLink()->GetName())
		{
			//�{�[���̌��݂̎p���ł̈ʒu���擾����
			*position = XMVector3TransformCoord(DirectX::g_XMZero, pBoneArray_[bone_i].newPose);
			
			return true;
		}
	}

	return false;
}

//���C�L���X�g
void FbxParts::RayCast(RayCastData* pData, Transform & transform) const
{
	if (pData == nullptr)	return;				//�����|�C���^��nullptr�ł���Ώ����s��

	//�}�e���A�������[�v
	for (DWORD material_i = 0; material_i < materialCount_; material_i++)
	{
		//�}�e���A���̃|���S���������[�v
		for (DWORD polygon_i = 0; polygon_i < indexCountEachMaterial_[material_i] / POLYGON_VERTICES; polygon_i++)
		{
			//���肷��|���S���̒��_���W��ݒ�
			XMVECTOR vertex[3];
			vertex[0] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 0]].position;	//�P�ڂ̒��_�̍��W
			vertex[1] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 1]].position;	//�Q�ڂ̒��_�̍��W
			vertex[2] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 2]].position;	//�R�ڂ̒��_�̍��W

			//���C�ƎO�p�|���S�����������Ă��邩����
			pData->isHit = Math::IntersectLineToPolygon(pData->start, pData->direction, vertex[0], vertex[1], vertex[2], &pData->distance);

			//�������Ă���I���
			if (pData->isHit) return;
		}
	}
}

//���
void FbxParts::Release()
{
	if (pWeightArray_ != nullptr)
	{
		for (DWORD i = 0; i < vertexCount_; i++)
		{
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneIndex);
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneWeight);
		}
		SAFE_DELETE_ARRAY(pWeightArray_);
	}

	SAFE_DELETE_ARRAY(pBoneArray_);
	SAFE_DELETE_ARRAY(ppCluster_);

	SAFE_DELETE_ARRAY(indexCountEachMaterial_);

	for (DWORD i = 0; i < materialCount_; i++)
	{
		SAFE_DELETE_ARRAY(ppIndexDatas_[i]);
		SAFE_RELEASE(pMaterialDatas_[i].pDiffuseTexture);
		SAFE_RELEASE(pMaterialDatas_[i].pNormalTexture);
		SAFE_RELEASE(ppIndexBuffer_[i]);
	}

	SAFE_DELETE_ARRAY(ppIndexDatas_);
	SAFE_DELETE_ARRAY(pVertexDatas_);
	SAFE_DELETE_ARRAY(pMaterialDatas_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_DELETE_ARRAY(ppIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
};

//���_�o�b�t�@����
HRESULT FbxParts::InitVertex(FbxMesh* pMesh)
{
	//���_�����N���X�ϐ��Ɋi�[
	SetVertexData(pMesh);

	//���_�o�b�t�@�̍쐬
	if (FAILED(CreateVertexBuffer())) return E_FAIL;

	return S_OK;
}

//���_�����N���X�ϐ��Ɋi�[
void FbxParts::SetVertexData(FbxMesh* pMesh)
{
	pVertexDatas_ = new VERTEX[vertexCount_];	//���_��������z��

	//�S�|���S��
	for (DWORD polygon_i = 0; polygon_i < polygonCount_; polygon_i++)
	{
		//�ڐ��E�@����񂪑��݂��邩���m�F
		int							startIndex		= pMesh->GetPolygonVertexIndex(polygon_i);			//�C���f�b�N�X�̊J�n�ʒu���擾
		FbxGeometryElementTangent*	pElementTangent = pMesh->GetElementTangent(0);						//�ڐ��v�f���擾
		bool						tangentExists	= pElementTangent != nullptr;						//�ڐ���񂪑��݂��邩
		bool						normalExists	= pMesh->GetLayer(0)->GetNormals();					//�@����񂪑��݂��邩

		//�ڐ������擾
		FbxVector4					tangent;															//�ڐ��v�f
		if(tangentExists) tangent = pElementTangent->GetDirectArray().GetAt(startIndex).mData;			//�ڐ���񂪑��݂�����ڐ��v�f����ڐ����̎擾

		//3���_��
		for (int vertex_i = 0; vertex_i < POLYGON_VERTICES; vertex_i++)
		{
			//���ׂ钸�_�̔ԍ�
			int index = pMesh->GetPolygonVertex(polygon_i, vertex_i);

			//���_�̈ʒu
			FbxVector4 vertexPosition = pMesh->GetControlPointAt(index);																				//���b�V����񂩂璸�_���W���擾
			pVertexDatas_[index].position = DirectX::XMVectorSet((float)-vertexPosition[0], (float)vertexPosition[1], (float)vertexPosition[2], 0.0f);	//���W���i�[

			//���_��UV
			FbxLayerElementUV* pUv = pMesh->GetLayer(0)->GetUVs();																						//���b�V����񂩂�uv�����擾
			int					uvIndex = pMesh->GetTextureUVIndex(polygon_i, vertex_i, FbxLayerElement::eTextureDiffuse);								//���b�V����񂩂�uv�C���f�b�N�X�����擾
			FbxVector2			uv = pUv->GetDirectArray().GetAt(uvIndex);																				//uv�l���擾
			pVertexDatas_[index].uv = DirectX::XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);								//UV�l���i�[

			//���_�̖@��
			if (normalExists)
			{
				FbxVector4 Normal;																														//�@�����
				pMesh->GetPolygonVertexNormal(polygon_i, vertex_i, Normal);																				//���b�V����񂩂�@�������擾
				pVertexDatas_[index].normal = DirectX::XMVectorSet((float)-Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);						//�@�����i�[
			}
			else
			{
				pVertexDatas_[index].normal = DirectX::g_XMZero;																						//�@����񂪖������0�x�N�g�����i�[
			}

			//���_�̐ڐ�
			if (tangentExists)
			{
				pVertexDatas_[index].tangent = DirectX::XMVectorSet((float)tangent[0], (float)tangent[1], (float)tangent[2], 0.0f);						//�ڐ����i�[
			}
			else 
			{
				pVertexDatas_[index].tangent = DirectX::g_XMZero;																						//�ڐ���񂪖������0�x�N�g�����i�[
			}
		}
	}
}

//���_�o�b�t�@�̍쐬
HRESULT FbxParts::CreateVertexBuffer()
{
	//���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth				= sizeof(VERTEX) * vertexCount_;	//�o�b�t�@�[�̃T�C�Y
	bd_vertex.Usage					= D3D11_USAGE_DYNAMIC;				//�o�b�t�@�[�̓ǂݎ�肨��я������݂��\�z�������@
	bd_vertex.BindFlags				= D3D11_BIND_VERTEX_BUFFER;			//�o�b�t�@�[���p�C�v���C���Ƀo�C���h�������@
	bd_vertex.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;			//CPU�A�N�Z�X�t���O
	bd_vertex.MiscFlags				= 0;								//���̑��̃t���O
	bd_vertex.StructureByteStride	= 0;								//�o�b�t�@�[�\�����̊e�v�f�̃T�C�Y

	//�������f�[�^�̐ݒ�
	D3D11_SUBRESOURCE_DATA data_vertex;									//�������f�[�^
	data_vertex.pSysMem = pVertexDatas_;								//�������f�[�^�ւ̃|�C���^

	//���_�o�b�t�@�̍쐬
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_)))
	{
		MessageBox(nullptr, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�C���f�b�N�X�o�b�t�@����
HRESULT FbxParts::InitIndex(FbxMesh* pMesh)
{
	ppIndexBuffer_			= new ID3D11Buffer*[materialCount_];	//�}�e���A���̌����������m��
	indexCountEachMaterial_ = new DWORD[materialCount_];			//�}�e���A���̌����������m��
	ppIndexDatas_			= new int*[materialCount_];				//�}�e���A���̌����������m��

	//�}�e���A���������[�v
	for (DWORD i = 0; i < materialCount_; i++)
	{
		//�C���f�b�N�X�����N���X�ϐ��Ɋi�[
		SetIndexData(pMesh, i);

		//�C���f�b�N�X�o�b�t�@�̍쐬
		if (FAILED(CreateIndexBuffer(i, indexCountEachMaterial_[i]))) return E_FAIL;
	}

	return S_OK;
}

//�C���f�b�N�X�����N���X�ϐ��Ɋi�[
void FbxParts::SetIndexData(FbxMesh * pMesh, int material)
{
	ppIndexDatas_[material] = new int[polygonCount_ * POLYGON_VERTICES];			//�|���S���� * �|���S���̒��_��
	int indexCount = 0;																//�C���f�b�N�X�̌�

	//�S�|���S��
	for (DWORD polygon_i = 0; polygon_i < polygonCount_; polygon_i++)
	{
		//�v�f�ւ̃C���f�b�N�X�ԍ����擾
		FbxLayerElementMaterial *pMaterial = pMesh->GetLayer(0)->GetMaterials();	//�}�e���A��
		assert(pMaterial != nullptr);
		int materialID = pMaterial->GetIndexArray().GetAt(polygon_i);				//�}�e���A����ID�ԍ�

		//�}�e���A����񂪈�v������|���S�����\�����钸�_�ԍ����C���f�b�N�X�ɒǉ�����
		if (materialID == material)
		{
			//3���_��
			for (int vertex_i = POLYGON_VERTICES - 1; vertex_i >= 0; vertex_i--)
			{
				ppIndexDatas_[material][indexCount] = pMesh->GetPolygonVertex(polygon_i, vertex_i);	//���_�ԍ����C���f�b�N�X�ɒǉ�
				indexCount++;																		//�C���f�b�N�X�̃J�E���g�𑝂₷
			}
		}
	}

	//�}�e���A���̃C���f�b�N�X�̌�
	indexCountEachMaterial_[material] = indexCount;
}

//�C���f�b�N�X�o�b�t�@�̍쐬
HRESULT FbxParts::CreateIndexBuffer(int material, int indexCount)
{
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;				//�o�b�t�@�[�̓ǂݎ�肨��я������݂��\�z�������@
	bd.ByteWidth		= sizeof(int) * indexCount;			//�o�b�t�@�[�̃T�C�Y
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;			//�o�b�t�@�[���p�C�v���C���Ƀo�C���h�������@
	bd.CPUAccessFlags	= 0;								//CPU�A�N�Z�X�t���O
	bd.MiscFlags		= 0;								//���̑��̃t���O

	//�������f�[�^�̐ݒ�
	D3D11_SUBRESOURCE_DATA InitData;						//�������f�[�^
	InitData.pSysMem = ppIndexDatas_[material];				//�������f�[�^�ւ̃|�C���^

	//�C���f�b�N�X�o�b�t�@�̍쐬
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd, &InitData, &ppIndexBuffer_[material])))
	{
		MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�R���X�^���g�o�b�t�@����
HRESULT FbxParts::InitConstantBuffer()
{
	//�R���X�^���g�o�b�t�@�쐬
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth			= sizeof(CONSTANT_BUFFER);		//�o�b�t�@�[�̃T�C�Y
	cb.Usage				= D3D11_USAGE_DYNAMIC;			//�o�b�t�@�[�̓ǂݎ�肨��я������݂��\�z�������@
	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;	//�o�b�t�@�[���p�C�v���C���Ƀo�C���h�������@
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		//CPU�A�N�Z�X�t���O
	cb.MiscFlags			= 0;							//���̑��̃t���O
	cb.StructureByteStride	= 0;							//�o�b�t�@�[�\�����̊e�v�f�̃T�C�Y

	//�R���X�^���g�o�b�t�@�̍쐬
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&cb, nullptr, &pConstantBuffer_)))
	{
		MessageBox(nullptr, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//�}�e���A������
HRESULT FbxParts::InitMaterial(FbxNode* pNode)
{
	pMaterialDatas_ = new MATERIAL[materialCount_];	//�}�e���A�������������m��

	//�}�e���A�������[�v
	for (DWORD i = 0; i < materialCount_; i++)
	{
		FbxSurfaceMaterial* pMaterial	= pNode->GetMaterial(i);		//i�Ԗڂ̃}�e���A�����
		FbxSurfacePhong*	pPhong		= (FbxSurfacePhong*)pMaterial;	//�t�H��
		FbxDouble3			ambient		= pPhong->Ambient;				//�A���r�G���g
		FbxDouble3			diffuse		= pPhong->Diffuse;				//�f�B�t���[�Y

		//�N���X�ϐ��Ɋi�[
		pMaterialDatas_[i].ambient		= XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
		pMaterialDatas_[i].diffuse		= XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		pMaterialDatas_[i].specular		= XMFLOAT4(0, 0, 0, 0);
		pMaterialDatas_[i].shininess	= 0;

		//�t�H���V�F�[�f�B���O���g�p���Ă��邩
		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			FbxDouble3 specular = pPhong->Specular;				//�X�y�L�����[

			//�N���X�ϐ��Ɋi�[
			pMaterialDatas_[i].specular		= XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);
			pMaterialDatas_[i].shininess	= (float)pPhong->Shininess;
		}

		//�g�p�e�N�X�`���̏���
		if (FAILED(InitDiffuseTexture(pMaterial, i))) return E_FAIL;

		//�@���}�b�v�̏���
		if (FAILED(InitNormalTexture(pMaterial, i))) return E_FAIL;
	}

	return S_OK;
}

//�e�N�X�`���̓ǂݍ���
HRESULT FbxParts::InitDiffuseTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material)
{
	pMaterialDatas_[material].pDiffuseTexture = nullptr;										//�i�[�p�e�N�X�`���̏�����

	//�e�N�X�`���̎g�p�������擾
	FbxProperty  property			= pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);	//�f�B�t���[�Y�̃v���p�e�B
	int			 fileTextureCount	= property.GetSrcObjectCount<FbxFileTexture>();				//�e�N�X�`���[�̖���

	if (!fileTextureCount) return S_OK;															//�e�N�X�`����񂪂Ȃ���Ή������Ȃ�

	//�e�N�X�`���t�@�C���܂ł̃p�X���擾
	FbxFileTexture* pTextureInfo	= property.GetSrcObject<FbxFileTexture>(0);					//�e�N�X�`���|���
	const char*		textureFilePath = pTextureInfo->GetRelativeFileName();						//�t�@�C�����i�p�X�j

	//�t�@�C����+�g���q�����ɂ���
	char name[_MAX_FNAME];																		//�t�@�C����
	char ext[_MAX_EXT];																			//�g���q
	_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);		//�t�@�C���p�X�𕪊�
	wsprintf(name, "%s%s", name, ext);															//�t�@�C�����Ɗg���q����̕�����ɂ���

	//�e�N�X�`���̓ǂݍ���
	pMaterialDatas_[material].pDiffuseTexture = new Texture;
	if (FAILED(pMaterialDatas_[material].pDiffuseTexture->Load(name))) return E_FAIL;

	return S_OK;
}

//�e�N�X�`���̓ǂݍ���
HRESULT FbxParts::InitNormalTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material)
{
	pMaterialDatas_[material].pNormalTexture = nullptr;										//�i�[�p�e�N�X�`���̏�����

	//�e�N�X�`���̎g�p�������擾
	FbxProperty  property = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);		//�f�B�t���[�Y�̃v���p�e�B
	int			 fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();			//�e�N�X�`���[�̖���

	if (!fileTextureCount) return S_OK;														//�e�N�X�`����񂪂Ȃ���Ή������Ȃ�

	//�e�N�X�`���t�@�C���܂ł̃p�X���擾
	FbxFileTexture* pTextureInfo = property.GetSrcObject<FbxFileTexture>(0);				//�e�N�X�`���|���
	const char* textureFilePath = pTextureInfo->GetRelativeFileName();						//�t�@�C�����i�p�X�j

	//�t�@�C����+�g���q�����ɂ���
	char name[_MAX_FNAME];																	//�t�@�C����
	char ext[_MAX_EXT];																		//�g���q
	_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);	//�t�@�C���p�X�𕪊�
	wsprintf(name, "%s%s", name, ext);														//�t�@�C�����Ɗg���q����̕�����ɂ���

	//�e�N�X�`���̓ǂݍ���
	pMaterialDatas_[material].pNormalTexture = new Texture;
	if (FAILED(pMaterialDatas_[material].pNormalTexture->Load(name))) return E_FAIL;

	return S_OK;
}

//�{�[���̏�������
void FbxParts::InitSkelton(FbxMesh* pMesh)
{
	// �f�t�H�[�}���i�{�[���ƃ��f���̊֘A�t���j�̎擾
	FbxDeformer* pDeformer = pMesh->GetDeformer(0);

	if (pDeformer == nullptr) return;					//�{�[����񂪂Ȃ���Ή������Ȃ�

	//�f�t�H�[�}��񂩂�X�L�����b�V�������擾
	pSkinInfo_ = (FbxSkin *)pDeformer;

	//�{�[���̐��ɍ��킹�ăE�F�C�g���쐬����
	CreateBoneWeight();

	//�{�[���̉e���͂𒸓_���Ƃɐ������Ċi�[
	SetBoneWeight();

	//�{�[���𐶐�
	CreateBone();
}

//�{�[���̐��ɍ��킹�ăE�F�C�g���쐬����
void FbxParts::CreateBoneWeight()
{
	//�{�[�������擾����
	numBone_	= pSkinInfo_->GetClusterCount();	//�{�[���̌����擾����
	ppCluster_	= new FbxCluster*[numBone_];		//�{�[���̌����N���X�^�[�����i�[���郁�������m��

	//�{�[�������̃N���X�^�[�����擾����
	for (int i = 0; i < numBone_; i++) ppCluster_[i] = pSkinInfo_->GetCluster(i);

	//�{�[���̐��ɍ��킹�ăE�F�C�g������������
	pWeightArray_ = new Weight[vertexCount_];
	for (DWORD vertex_i = 0; vertex_i < vertexCount_; vertex_i++)
	{
		pWeightArray_[vertex_i].positionOrigin	= pVertexDatas_[vertex_i].position;	//�ό`���Ă��Ȃ���Ԃ̒��_���W
		pWeightArray_[vertex_i].normalOrigin	= pVertexDatas_[vertex_i].normal;	//�ό`���Ă��Ȃ���Ԃ̒��_�̖@��
		pWeightArray_[vertex_i].pBoneIndex		= new int[numBone_];				//�{�[���̌����C���f�b�N�X�����i�[���郁�������m��
		pWeightArray_[vertex_i].pBoneWeight		= new float[numBone_];				//�{�[���̌����E�F�C�g�����i�[���郁�������m��

		//�S�{�[���̃E�F�C�g���̏�����
		for (int bone_i = 0; bone_i < numBone_; bone_i++)
		{
			pWeightArray_[vertex_i].pBoneIndex[bone_i]	= -1;
			pWeightArray_[vertex_i].pBoneWeight[bone_i] = 0.f;
		}
	}
}

//�{�[���̃E�F�C�g���𒸓_���Ƃɐ������Ċi�[
void FbxParts::SetBoneWeight()
{
	const int MAX_BIND_BONE = 4;			//1���_�Ɋ֘A�t������{�[���̍ő吔

	//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
	//����𒸓_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮗�����
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//�{�[���̏����擾
		int		indexCount	= ppCluster_[bone_i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
		int*	pIndex		= ppCluster_[bone_i]->GetControlPointIndices();		//�{�[��/�E�F�C�g���̔ԍ�
		double* pWeight		= ppCluster_[bone_i]->GetControlPointWeights();		//���_���Ƃ̃E�F�C�g���

		//���_������C���f�b�N�X�����ǂ��āA���_���Ő�������
		//���_�������[�v
		for (int vertex_i = 0; vertex_i < indexCount; vertex_i++)
		{
			//���_�Ɋ֘A�t���邱�Ƃ��ł���{�[���̍ő吔�����[�v
			for (int bindbone_i = 0; bindbone_i < MAX_BIND_BONE; bindbone_i++)
			{
				if (bindbone_i >= numBone_)	break;						//Fbx�̃{�[����S�Č��I�������

				//���̒��_�ɑ΂���E�F�C�g�����Ɋ֘A�t����ꂽ���̂��傫����΍~���ɂȂ�悤�ɓ���ւ���
				if (pWeight[vertex_i] > pWeightArray_[pIndex[vertex_i]].pBoneWeight[bindbone_i])
				{
					//pWeight[vertex]�̊i�[�ʒu�ȍ~�Ɋi�[����Ă���l�̗v�f�������ɂ��炷
					for (int i = numBone_ - 1; i > bindbone_i; i--)
					{
						pWeightArray_[pIndex[vertex_i]].pBoneIndex[i]	= pWeightArray_[pIndex[vertex_i]].pBoneIndex[i - 1];	//�C���f�b�N�X���i�[����v�f�����P���ɂ��炷
						pWeightArray_[pIndex[vertex_i]].pBoneWeight[i]	= pWeightArray_[pIndex[vertex_i]].pBoneWeight[i - 1];	//�E�F�C�g���i�[����v�f�����P���ɂ��炷
					}

					//�{�[����V�����֘A�t����
					pWeightArray_[pIndex[vertex_i]].pBoneIndex[bindbone_i]	= bone_i;					//�V�����֘A�t����{�[��
					pWeightArray_[pIndex[vertex_i]].pBoneWeight[bindbone_i] = (float)pWeight[vertex_i];	//�V�����֘A�t����{�[���̒��_�ɑ΂���E�F�C�g
					break;
				}
			}
		}
	}
}

//�{�[���𐶐�
void FbxParts::CreateBone()
{
	pBoneArray_ = new Bone[numBone_];						//�{�[���̔z��̃������m��

	//�{�[���̌������[�v
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//�{�[���̃f�t�H���g�ʒu���擾����
		FbxAMatrix  matrix;									//Fbx�`���̊֐ߍs��
		ppCluster_[bone_i]->GetTransformLinkMatrix(matrix);	//�{�[���̃f�t�H���g�ʒu���擾

		//�֐ߍs���Fbx�`������DirectX�֕ϊ��R�s�[����
		XMFLOAT4X4 pose;									//DirectX�`���̊֐ߍs��
		for (DWORD y = 0; y < 4; y++) {
			for (DWORD x = 0; x < 4; x++) {
				pose(x, y) = (float)matrix.Get(x, y);
			}

			//�E��n(Maya)���獶��n(DirectX)�֕ϊ�
			pose.m[y][0] *= -1.f;
		}

		//�{�[���̃f�t�H���g�ʒu���i�[
		pBoneArray_[bone_i].bindPose = XMLoadFloat4x4(&pose);
	}
}

//�}�e���A�����Ƃɕ`��
void FbxParts::DrawMaterial(Transform & transform) const
{
	D3D11_MAPPED_SUBRESOURCE	data;
	ID3D11DeviceContext*		pContext = Direct3D::GetContext();	//�f�o�C�X�R���e�L�X�g�̃|�C���^

	//�}�e���A�������[�v
	for (DWORD material_i = 0; material_i < materialCount_; material_i++)
	{
		//�C���f�b�N�X�o�b�t�@�[���Z�b�g
		pContext->IASetIndexBuffer(ppIndexBuffer_[material_i], DXGI_FORMAT_R32_UINT, 0);

		//�R���X�^���g�o�b�t�@�ɓn����������
		CONSTANT_BUFFER cb;
		cb.vertexConversionMatrix	= XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
		cb.normalConversionMatrix	= XMMatrixTranspose(transform.GetRotateMatrix() * XMMatrixInverse(nullptr, transform.GetScaleMatrix()));
		cb.matWorld					= transform.GetWorldMatrix();
		cb.ambient					= pMaterialDatas_[material_i].ambient;
		cb.diffuseColor				= pMaterialDatas_[material_i].diffuse;
		cb.speculer					= pMaterialDatas_[material_i].specular;
		cb.shininess				= pMaterialDatas_[material_i].shininess;
		cb.isTexture				= pMaterialDatas_[material_i].pDiffuseTexture != nullptr;
		XMStoreFloat4(&cb.cameraPosition, Camera::GetPosition());

		//�e�N�X�`��������ΐݒ�
		if (cb.isTexture)
		{
			//�T���v���[�i�e�N�X�`���̕\���̎d���j��ݒ�
			ID3D11SamplerState*			pSampler = pMaterialDatas_[material_i].pDiffuseTexture->GetSampler();
			pContext->PSSetSamplers(0, 1, &pSampler);

			//�V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j��ݒ�
			ID3D11ShaderResourceView*	pSRV = pMaterialDatas_[material_i].pDiffuseTexture->GetSRV();
			pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		//���݂̃V�F�[�_���n�ʕ`��p�ł���Βn�ʕ`��ɕK�v�ȍ��ڂ�ݒ肷��
		if (Direct3D::GetCurrentShaderType() == Direct3D::SHADER_TYPE::GROUND)
		{
			//�m�[�}���}�b�v�e�N�X�`�����V�F�[�_�[���\�[�X�r���[�ɐݒ�
			ID3D11ShaderResourceView* pSRV = pMaterialDatas_[material_i].pNormalTexture->GetSRV();
			pContext->PSSetShaderResources(1, 1, &pSRV);
		}

		//�R���X�^���g�o�b�t�@�̏�������
		pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);	//GPU����̃f�[�^�A�N�Z�X���~�߂�
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));			//�f�[�^�l�𑗂�
		pContext->Unmap(pConstantBuffer_, 0);									//�ĊJ

		//�|���S�����b�V���̕`��
		pContext->DrawIndexed(indexCountEachMaterial_[material_i], 0, 0);
	}
}