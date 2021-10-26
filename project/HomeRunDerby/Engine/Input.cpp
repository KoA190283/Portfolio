#include "Input.h"

#include "Global.h"

namespace Input
{
	namespace
	{
		//const char			
		const BYTE				KEY				= 0x80;		//���͂���Ă��邩�𔻒f����r�b�g�ʒu

		LPDIRECTINPUT8			pDxInput_		= nullptr;	//DxInput Ver8�^�̃|�C���^

		//�L�[�{�[�h�֘A
		LPDIRECTINPUTDEVICE8	pKeyDevice_		= nullptr;	//�L�[�{�[�h�f�o�C�X
		BYTE					keyState_[256];				//�L�[�{�[�h�̏�ԁi������Ă��邩�j������
		BYTE					prevKeyState_[256];			//�O�t���[���ł̊e�L�[�̏��

		//�}�E�X�֘A
		LPDIRECTINPUTDEVICE8	pMouseDevice_	= nullptr;	//�}�E�X�f�o�C�X
		DIMOUSESTATE			mouseState_;				//�}�E�X�̏�ԁi������Ă��邩�j������
		DIMOUSESTATE			prevMouseState_;			//�O�t���[���ł̊e�}�E�X�̏��
		XMVECTOR				mousePosition_;				//�}�E�X�̈ʒu

		//�R���g���[���[�֘A
		XINPUT_STATE			controllerState_;			//�R���g���[���[�̏�ԁi������Ă��邩�j������
		XINPUT_STATE			prevControllerState_;		//�O�t���[���ł̊e�R���g���[���[�̏��
	}

	//������
	HRESULT Initialize(const HWND& hWnd)
	{
		//DirectInput�I�u�W�F�N�g���쐬
		if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDxInput_, nullptr)))
		{
			MessageBox(nullptr, "DirectInput�I�u�W�F�N�g�쐬�Ɏ��s���܂���", "�G���[", MB_OK);
			return E_FAIL;
		}

		//�L�[�{�[�h�p�̃f�o�C�X�I�u�W�F�N�g���쐬
		if (FAILED(pDxInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr)))
		{
			MessageBox(nullptr, "�L�[�{�[�h�p�̃f�o�C�X�I�u�W�F�N�g�Ɏ��s���܂���", "�G���[", MB_OK);

			return E_FAIL;
		}
		pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);									//�f�o�C�X���L�[�{�[�h�Ɏw��
		pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);	//�D��x��ݒ�

		//�}�E�X�p�̃f�o�C�X�I�u�W�F�N�g���쐬
		if (FAILED(pDxInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr)))
		{
			MessageBox(nullptr, "�}�E�X�p�̃f�o�C�X�I�u�W�F�N�g�Ɏ��s���܂���", "�G���[", MB_OK);

			return E_FAIL;
		}
		pMouseDevice_->SetDataFormat(&c_dfDIMouse);										//�f�o�C�X���}�E�X�Ɏw��
		pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);//�D��x��ݒ�

		return S_OK;

	}

	//�X�V
	void Update()
	{
		//�L�[�{�[�h�̏�Ԃ̍X�V
		pKeyDevice_->Acquire();														//�L�[�{�[�h�擾
		memcpy(prevKeyState_, keyState_, sizeof(keyState_));						//keyState_ -> prevKeyState_�ɓ����
		pKeyDevice_->GetDeviceState(sizeof(keyState_), &keyState_);					//�L�[�{�[�h�̍��̑S�L�[�̏�Ԃ�����

		//�}�E�X�̏�Ԃ̍X�V
		pMouseDevice_->Acquire();													//�}�E�X�擾
		memcpy(&prevMouseState_, &mouseState_, sizeof(mouseState_));				//mouseState_ -> prevMouseState_�ɓ����
		pMouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);			//�}�E�X�̍��̏�Ԃ�����

		//�R���g���[���[�̏�Ԃ��擾
		memcpy(&prevControllerState_, &controllerState_, sizeof(controllerState_));	//controllerState_ -> prevControllerState_�ɓ����
		XInputGetState(0, &controllerState_);										//�R���g���[���[�̍��̏�Ԃ�����
	}

	//�L�[��������Ă��邩�𒲂ׂ�
	bool IsKey(int keyCode)
	{
		//������Ă���true��Ԃ�
		if (keyState_[keyCode] & KEY) return true;

		return false;
	}

	//�L�[���������ꂽ���𒲂ׂ�
	bool IsKeyDown(int keyCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ��Ȃ�true��Ԃ�
		if (IsKey(keyCode) && !(prevKeyState_[keyCode] & KEY)) return true;

		return false;
	}

	//�L�[���������ꂽ���𒲂ׂ�
	bool IsKeyUp(int keyCode)
	{
		//���͉����ĂȂ��āA�O��͉����Ă�Ȃ�true��Ԃ�
		if (!IsKey(keyCode) && prevKeyState_[keyCode] & KEY)	return true;

		return false;
	}

	//�}�E�X��������Ă��邩�𒲂ׂ�
	bool IsMouseButton(int buttonCode)
	{
		//������Ă�����true��Ԃ�
		if (mouseState_.rgbButtons[buttonCode] & KEY) return true;

		return false;
	}

	//�}�E�X���������ꂽ���𒲂ׂ�
	bool IsMouseButtonDown(int buttonCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ������true��Ԃ�
		if (IsMouseButton(buttonCode) && !(prevMouseState_.rgbButtons[buttonCode] & KEY)) return true;
		
		return false;
	}

	//�}�E�X���������ꂽ���𒲂ׂ�
	bool IsMouseButtonUp(int buttonCode)
	{
		//���͉����ĂȂ��āA�O��͉����Ă�Ȃ�true��Ԃ�
		if (!IsMouseButton(buttonCode) && prevMouseState_.rgbButtons[buttonCode] & KEY) return true;
		
			return false;
	}

	//�}�E�X�̈ʒu������
	void SetMousePosition(int x, int y)
	{
		mousePosition_ = DirectX::XMVectorSet((float)x, (float)y, 0, 0);
	}

	//�}�E�X�̈ʒu�����擾
	XMVECTOR GetMousePosition()
	{
		return mousePosition_;
	}

	//�}�E�X�̈ړ��ʂ��擾
	XMVECTOR GetMouseMove()
	{
		return DirectX::XMVectorSet((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ, 0);
	}

	//�Q�[���p�b�h��������Ă��邩�𒲂ׂ�
	bool IsPadButton(int buttonCode)
	{
		//������Ă���Ȃ�true��Ԃ�
		if (controllerState_.Gamepad.wButtons & buttonCode) return true;
		
		return false;
	}

	//�Q�[���p�b�h���������ꂽ���𒲂ׂ�
	bool IsPadButtonDown(int buttonCode)
	{
		//���͉����ĂāA�O��͉����ĂȂ��Ȃ�true��Ԃ�
		if (IsPadButton(buttonCode) && !(prevControllerState_.Gamepad.wButtons & buttonCode)) return true;
		
		return false;
	}

	//�Q�[���p�b�h���������ꂽ���𒲂ׂ�
	bool IsPadButtonUp(int buttonCode)
	{
		//���͉����ĂȂ��āA�O��͉����Ă�Ȃ�true��Ԃ�
		if (!IsPadButton(buttonCode) && prevControllerState_.Gamepad.wButtons & buttonCode) return true;
		
		return false;
	}

	//�R���g���[���X�e�B�b�N�̌X���̔䗦���擾
	float GetAnalogValue(int analog, int max, int deadZone)
	{
		float result = (float)analog;	//�R���g���[���X�e�B�b�N�̌X��

		//�X����deadZone�����Ȃ�X���Ƃ��đ����Ȃ�
		if (fabs(result) < deadZone)	return 0;

		//�X���̕����ɍ��킹�ČX����-1~1�Œ���
		if (result > 0)	result = (result - deadZone) / (max - deadZone);	// 0����1�ŌX��������
		else			result = (result + deadZone) / (max - deadZone);	//-1����0�ŌX��������

		return result;
	}

	//���R���g���[���X�e�B�b�N�̌X�����擾
	XMVECTOR GetPadStickL()
	{
		float x = GetAnalogValue(controllerState_.Gamepad.sThumbLX, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_.Gamepad.sThumbLY, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		return DirectX::XMVectorSet(x, y, 0, 0);
	}

	//�E�R���g���[���X�e�B�b�N�̌X�����擾
	XMVECTOR GetPadStickR()
	{
		float x = GetAnalogValue(controllerState_.Gamepad.sThumbRX, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_.Gamepad.sThumbRY, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		return DirectX::XMVectorSet(x, y, 0, 0);
	}

	//���g���K�[�{�^���̌X�����擾
	float GetPadTriggerL()
	{
		return GetAnalogValue(controllerState_.Gamepad.bLeftTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	//�E�g���K�[�{�^���̌X�����擾
	float GetPadTriggerR()
	{
		return GetAnalogValue(controllerState_.Gamepad.bRightTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	//�U���G�t�F�N�g
	void SetPadVibration(float left, float right)
	{
		const int			MAX_VIBRATION = 65535;					//���[�^�[�̍ő�l
		XINPUT_VIBRATION	vibration;								//���[�^�[�̋���

		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));			//������
		vibration.wLeftMotorSpeed	= (int)left * MAX_VIBRATION;	//�����[�^�[�̋���
		vibration.wRightMotorSpeed	= (int)right * MAX_VIBRATION;	//�E���[�^�[�̋���
		XInputSetState(0, &vibration);								//����
	}

	//���
	void Release()
	{
		SAFE_RELEASE(pMouseDevice_);
		SAFE_RELEASE(pKeyDevice_);
		SAFE_RELEASE(pDxInput_);
	}
}

