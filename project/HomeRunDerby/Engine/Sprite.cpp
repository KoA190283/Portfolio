#include "Sprite.h"

#include "Global.h"
#include "Texture.h"

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

Sprite::Sprite()
{
	pVertexBuffer_		= nullptr;		//���_�o�b�t�@
	pIndexBuffer_		= nullptr;		//�C���f�b�N�X�o�b�t�@
	pConstantBuffer_	= nullptr;		//�R���X�^���g�o�b�t�@
	pTexture_			= nullptr;		//�e�N�X�`��
	indexNum_			= 0;			//�C���f�b�N�X��
}

Sprite::~Sprite()
{
	Release();
}

//�摜�f�[�^���������߂̏���
HRESULT Sprite::Initialize(const std::string& filename)
{
	//���_�o�b�t�@����
	if (FAILED(InitVertex())) return E_FAIL;

	//�C���f�b�N�X�o�b�t�@����
	if (FAILED(InitIndex())) return E_FAIL;
	
	//�R���X�^���g�o�b�t�@����
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//�e�L�X�`���[�̃��[�h
	pTexture_ = new Texture;
	if (FAILED(pTexture_->Load(filename.c_str()))) return E_FAIL;
	
	return S_OK;
}

//�摜�f�[�^���������߂̏���
HRESULT Sprite::Initialize(ID3D11Texture2D*	pTexture)
{
	//���_�o�b�t�@����
	if (FAILED(InitVertex())) return E_FAIL;

	//�C���f�b�N�X�o�b�t�@����
	if (FAILED(InitIndex())) return E_FAIL;

	//�R���X�^���g�o�b�t�@����
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//�e�L�X�`���[�̃��[�h
	pTexture_ = new Texture;
	if (FAILED(pTexture_->InitializeTexture2D(pTexture))) return E_FAIL;

	return S_OK;
}

//�`��
void Sprite::Draw(Transform & transform) const
{
	ID3D11DeviceContext*	pContext	= Direct3D::GetContext();	//�f�o�C�X�R���e�L�X�g�̃|�C���^
	static float			scroll		= 0.f;						//�o�ߎ���

	//���݂̃V�F�[�_����`��p�̂��̂ł���΃X�N���[���ϐ��̒l��0.001�傫������
	if (Direct3D::GetCurrentShaderType() == Direct3D::SHADER_TYPE::SKY_2D) scroll += 0.001f;

	//�R���X�^���g�o�b�t�@�ɓn���l�̏���
	CONSTANT_BUFFER cb;
	XMMATRIX		adjustScale	= DirectX::XMMatrixScaling((float)pTexture_->GetWidth(), (float)pTexture_->GetHeight(), 1.f);												//�摜�T�C�Y
	XMMATRIX		miniScale	= DirectX::XMMatrixScaling((1.f / Global::WINDOW_WIDTH), (1.f / Global::WINDOW_HEIGHT), 1.f);												//�E�B���h�E�T�C�Y
	cb.vertexConversionMatrix	= XMMatrixTranspose(adjustScale * transform.GetScaleMatrix() * transform.GetRotateMatrix() * miniScale * transform.GetTranslateMatrix());	//�e�T�C�Y�ƕ`�悷���Ԃ���Z
	cb.scroll					= scroll;

	//�T���v���[�i�e�N�X�`���̕\���̎d���j��ݒ�
	ID3D11SamplerState*			pSampler = pTexture_->GetSampler();
	pContext->PSSetSamplers(0, 1, &pSampler);

	//�V�F�[�_�[���\�[�X�r���[�i�e�N�X�`���j��ݒ�
	ID3D11ShaderResourceView*	pSRV	 = pTexture_->GetSRV();
	pContext->PSSetShaderResources(0, 1, &pSRV);

	//���_�o�b�t�@���Z�b�g
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//�C���f�b�N�X�o�b�t�@�[���Z�b�g
	stride		= sizeof(int);
	offset		= 0;
	pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	//�R���X�^���g�o�b�t�@
	pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);					//���_�V�F�[�_�[�p
	pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);					//�s�N�Z���V�F�[�_�[�p

	//�R���X�^���g�o�b�t�@�̏�������
	D3D11_MAPPED_SUBRESOURCE	data;
	pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);		//GPU����̃f�[�^�A�N�Z�X���~�߂�
	memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));				//�f�[�^�l�𑗂�
	pContext->Unmap(pConstantBuffer_, 0);										//�ĊJ

	//�`��												
	pContext->DrawIndexed(indexNum_, 0, 0);
}

UINT Sprite::GetWidth() const
{
	return pTexture_->GetWidth();
}

UINT Sprite::GetHeight() const
{
	return pTexture_->GetHeight();
}

//���
void Sprite::Release()
{
	SAFE_RELEASE(pTexture_);
	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}

//���_���̐ݒ�
HRESULT Sprite::InitVertex()
{
	using DirectX::XMVectorSet;

	//���_���
	VERTEX vertices[] =
	{
		{ XMVectorSet(-1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f) },   // �l�p�`�̒��_�i����j
		{ XMVectorSet( 1.0f,  1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) },   // �l�p�`�̒��_�i�E��j
		{ XMVectorSet( 1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f) },   // �l�p�`�̒��_�i�E���j
		{ XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) },   // �l�p�`�̒��_�i�����j
	};

	//���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth				= sizeof(vertices);			//�o�b�t�@�[�̃T�C�Y
	bd_vertex.Usage					= D3D11_USAGE_DEFAULT;		//�o�b�t�@�[�̓ǂݎ�肨��я������݂��\�z�������@
	bd_vertex.BindFlags				= D3D11_BIND_VERTEX_BUFFER;	//�o�b�t�@�[���p�C�v���C���Ƀo�C���h�������@
	bd_vertex.CPUAccessFlags		= 0;						//CPU�A�N�Z�X�t���O
	bd_vertex.MiscFlags				= 0;						//���̑��̃t���O
	bd_vertex.StructureByteStride	= 0;						//�o�b�t�@�[�\�����̊e�v�f�̃T�C�Y

	D3D11_SUBRESOURCE_DATA data_vertex;							//�������p�f�[�^
	data_vertex.pSysMem = vertices;								//�������f�[�^�ւ̃|�C���^

	//���_�o�b�t�@�̍쐬
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_)))
	{
		MessageBox(nullptr, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
//���_���̐ݒ�
HRESULT Sprite::InitIndex()
{
	int index[] = { 0,2,3,  0,1,2 };					//�C���f�b�N�X���
	indexNum_	= sizeof(index) / sizeof(int);			//�C���f�b�N�X��

	// �C���f�b�N�X�o�b�t�@�𐶐�����
	D3D11_BUFFER_DESC   bd;
	bd.ByteWidth			= sizeof(index);			//�o�b�t�@�[�̃T�C�Y
	bd.Usage				= D3D11_USAGE_DEFAULT;		//�o�b�t�@�[�̓ǂݎ�肨��я������݂��\�z�������@
	bd.BindFlags			= D3D11_BIND_INDEX_BUFFER;	//�o�b�t�@�[���p�C�v���C���Ƀo�C���h�������@
	bd.CPUAccessFlags		= 0;						//CPU�A�N�Z�X�t���O
	bd.MiscFlags			= 0;						//���̑��̃t���O

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem			= index;				//�������f�[�^�ւ̃|�C���^
	initData.SysMemPitch		= 0;					//�e�N�X�`����1�̍s�̐擪���玟�̍s�܂ł̋���
	initData.SysMemSlicePitch	= 0;					//����[�x���x���̐擪���玟�̐[�x���x���܂ł̋���
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd, &initData, &pIndexBuffer_)))
	{
		MessageBox(nullptr, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//���_���̐ݒ�
HRESULT Sprite::InitConstantBuffer()
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
