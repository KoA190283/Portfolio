#pragma once
#include <d3d11.h>
#include <assert.h>

//�����J
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//3D�`����s�����߂ɕK�v�Ȃ��̂��Ǘ�
namespace Direct3D
{
	//�V�F�[�_�[�̎��
	enum class SHADER_TYPE
	{
		DEFAULT_3D = 0,		//3D�`��p
		DEFAULT_2D,			//2D�`��p
		NO_SHADE_3D,		//�V�F�[�h���g�p���Ȃ�3D�`��
		SKY_2D,				//�w�i�̋�`��p
		GROUND,				//�n�ʕ`��p
		MAX
	};

	//������
	//�����FhWnd �E�B���h�E�n���h��
	//�ߒl�F�����̐���
	HRESULT Initialize(HWND &hWnd);

	//���ݒ�
	//�����FscDesc �ݒ�p�\����
	//�����FhWnd �E�B���h�E�n���h��
	//�ߒl�F�����̐���
	HRESULT InitAdvance(HWND &hWnd);

	//�r���[�̏���
	//�ߒl�F�����̐���
	HRESULT InitView();

	//�V�F�[�_�[����
	//�ߒl�F�����̐���
	HRESULT InitShader();

	//�g�p����V�F�[�_�[��ݒ�
	//�����Ftype �V�F�[�_�[�̎��
	void SetShaderBundle(SHADER_TYPE type);

	//3D�`��p�̃V�F�[�_����
	//�ߒl�F�����̐���
	HRESULT InitShader3D();

	//2D�`��p�̃V�F�[�_����
	//�ߒl�F�����̐���
	HRESULT InitShader2D();

	//�V�F�[�h���g�p���Ȃ�3D�`��p�̃V�F�[�_����
	//�ߒl�F�����̐���
	HRESULT InitNoShadeShader3D();

	//�w�i�̋�`��p�̃V�F�[�_����
	//�ߒl�F�����̐���
	HRESULT InitShaderSky();

	//�n�ʕ`��p�̃V�F�[�_����
	//�ߒl�F�����̐���
	HRESULT InitShaderGround();

	//�`��J�n
	void BeginDraw();

	//�`��I��
	void EndDraw();

	//���
	void Release();

	////////////////////
	// �擾�֐�
	ID3D11Device*		 GetDevice();	//�f�o�C�X���擾
	ID3D11DeviceContext* GetContext();	//�f�o�C�X�R���e�L�X�g���擾
	SHADER_TYPE GetCurrentShaderType();	//���ݐݒ肵�Ă���V�F�[�_�[�^�C�v���擾
};

