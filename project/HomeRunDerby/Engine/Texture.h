#pragma once
#include <d3d11.h>
#include <wincodec.h>

#include "string"

#pragma comment( lib, "WindowsCodecs.lib" )

//�e�N�X�`���N���X
class Texture
{
private:

	ID3D11SamplerState*			pSampler_;	//�T���v���[
	ID3D11ShaderResourceView*	pSRV_;		//�V�F�[�_�[���\�[�X�r���[

	UINT						imgWidth_;	//�摜�T�C�Y�@��
	UINT						imgHeight_;	//�摜�T�C�Y�@����

public:

	Texture();
	~Texture();

	//�摜�t�@�C���̃��[�h
	//�����FfileName �t�@�C����
	//�ߒl�F�����̐���
	HRESULT Load(const std::string& fileName);

	//�e�N�X�`���̏�����
	//�����FpTexture			�e�N�X�`���f�[�^
	//�ߒl�F�����̐���
	HRESULT InitializeTexture2D(ID3D11Texture2D * pTexture);

	//���
	//�����F�Ȃ�
	//�ߒl�F�Ȃ�
	void Release();

	//�擾�֐�
	ID3D11SamplerState*			GetSampler() const;	//�T���v���[
	ID3D11ShaderResourceView*	GetSRV() const;		//�V�F�[�_�[���\�[�X�r���[
	UINT						GetWidth() const;	//��
	UINT						GetHeight() const;	//����

private:

	//�t�@�C�����J��
	//�����FfileName			�t�@�C����
	//�����FpFormatConverter	�R���o�[�^�[
	//�ߒl�F�����̐���
	HRESULT InitCOM(const std::string& fileName, IWICFormatConverter*& pFormatConverter);

	//�e�N�X�`���̍쐬	
	//�����FpTexture	�e�N�X�`��
	//�ߒl�F�����̐���
	HRESULT CreateTexture(ID3D11Texture2D*&	pTexture);

	//�T���v���[�̍쐬
	//�ߒl�F�����̐���
	HRESULT CreateSampler();

	//�V�F�[�_�[���\�[�X�r���[�̍쐬
	//�����FpTexture			�e�N�X�`���f�[�^
	//�ߒl�F�����̐���
	HRESULT CreateShaderResourceView(ID3D11Texture2D* pTexture);
};