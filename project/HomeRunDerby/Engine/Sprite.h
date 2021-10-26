#pragma once
#include <DirectXMath.h>

#include "Direct3D.h"
#include "Texture.h"
#include "Transform.h"

//2D�I�u�W�F�N�g�i�摜�j�N���X
class Sprite
{
private:

	//�R���X�^���g�o�b�t�@�[
	struct CONSTANT_BUFFER
	{
		using XMMATRIX = DirectX::XMMATRIX;

		XMMATRIX vertexConversionMatrix;	//���_���W�ϊ��s��
		FLOAT	 scroll;					//�X�N���[��
	};

	//���_���
	struct VERTEX
	{
		using XMVECTOR = DirectX::XMVECTOR;

		XMVECTOR position;			//��ʓ��̒��_���W
		XMVECTOR uv;				//UV���W
	};

	ID3D11Buffer*	pVertexBuffer_;		//���_�o�b�t�@
	ID3D11Buffer*	pIndexBuffer_;		//�C���f�b�N�X�o�b�t�@
	ID3D11Buffer*	pConstantBuffer_;	//�R���X�^���g�o�b�t�@
	Texture*		pTexture_;			//�e�N�X�`��
	int				indexNum_;			//�C���f�b�N�X��

public:

	Sprite();
	~Sprite();

	//�摜�f�[�^���������߂̏���
	//�����FfileName	�\������摜�t�@�C��
	//�ߒl�F�����̐���
	HRESULT Initialize(const std::string& fileName);

	//�摜�f�[�^���������߂̏���
	//�����FpTexture	�e�N�X�`��
	//�ߒl�F�����̐���
	HRESULT Initialize(ID3D11Texture2D*	pTexture);

	//�`��
	//�����FTransform�@���[���h���W
	void Draw(Transform& Transform) const;

	//�擾�֐�
	UINT GetWidth() const;	//��
	UINT GetHeight() const;	//����

	//���
	void Release();

private:

	//���_���̐ݒ�
	//�ߒl�F�����̐���
	HRESULT InitVertex();

	//���_���̐ݒ�
	//�ߒl�F�����̐���
	HRESULT InitIndex();

	//���_���̐ݒ�
	//�ߒl�F�����̐���
	HRESULT InitConstantBuffer();
};