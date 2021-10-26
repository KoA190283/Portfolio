#pragma once
#include <dInput.h>
#include <DirectXMath.h>

#include "XInput.h"				

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")	
#pragma comment(lib, "Xinput.lib")	

//���͊Ǘ�
namespace Input
{
	using DirectX::XMVECTOR;

	//������
	//�����FhWnd �E�B���h�E�n���h��
	//�ߒl�F�����̐���
	HRESULT Initialize(const HWND&);

	//�X�V
	//�����F�Ȃ�
	//�ߒl�F�Ȃ�
	void Update();

	//�L�[��������Ă��邩�𒲂ׂ�
	//�����FkeyCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsKey(int keyCode);

	//�L�[���������ꂽ���𒲂ׂ�
	//�����FkeyCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsKeyDown(int keyCode);

	//�L�[���������ꂽ���𒲂ׂ�
	//�����FkeyCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsKeyUp(int keyCode);

	//�}�E�X��������Ă��邩�𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsMouseButton(int buttonCode);

	//�}�E�X���������ꂽ���𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsMouseButtonDown(int buttonCode);

	//�}�E�X���������ꂽ���𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsMouseButtonUp(int buttonCode);

	//�}�E�X�̈ʒu���Z�b�g����
	//�����Fx �}�E�X��x���W
	//�����Fy �}�E�X��y���W
	//�ߒl�F�Ȃ�
	void SetMousePosition(int x, int y);

	//�}�E�X�̍��W���擾
	//�ߒl�F�}�E�X�̍��W
	XMVECTOR GetMousePosition();

	//�}�E�X�̈ړ��ʂ��擾
	//�ߒl�F�}�E�X�̈ړ���
	XMVECTOR GetMouseMove();

	//�Q�[���p�b�h��������Ă��邩�𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsPadButton(int buttonCode);

	//�Q�[���p�b�h���������ꂽ���𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsPadButtonDown(int buttonCode);

	//�Q�[���p�b�h���������ꂽ���𒲂ׂ�
	//�����FbuttonCode ���肷��L�[�̔ԍ�
	//�ߒl�F�^�U
	bool IsPadButtonUp(int buttonCode);

	//�R���g���[���X�e�B�b�N�̌X���̔䗦���擾
	//�����Fanalog�@�X�e�B�b�N�̌X��
	//�����Fmax�@�X�e�B�b�N�̌X���̍ő�l
	//�����FdeadZone�@�X�e�B�b�N�̗V��
	//�ߒl�F�X�e�B�b�N�̌X���̔䗦
	float GetAnalogValue(int analog, int max, int deadZone);

	//���R���g���[���X�e�B�b�N�̌X�����擾
	//�ߒl�F���R���g���[���X�e�B�b�N�̌X��
	XMVECTOR GetPadStickL();

	//�E�R���g���[���X�e�B�b�N�̌X�����擾
	//�ߒl�F�E�R���g���[���X�e�B�b�N�̌X��
	XMVECTOR GetPadStickR();

	//���g���K�[�{�^���̌X�����擾
	//�ߒl�F���R���g���[���X�e�B�b�N�̌X��
	float	GetPadTriggerL();
	
	//�E�g���K�[�{�^���̌X�����擾
	//�ߒl�F�E�R���g���[���X�e�B�b�N�̌X��
	float	GetPadTriggerR();
	
	//�U���G�t�F�N�g
	//�����Fleft�@���̐U���̑傫�� 0-1
	//�����Fright �E�̐U���̑傫�� 0-1
	void SetPadVibration(float left, float right);

	//���
	void Release();
};