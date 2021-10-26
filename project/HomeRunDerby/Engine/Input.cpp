#include "Input.h"

#include "Global.h"

namespace Input
{
	namespace
	{
		//const char			
		const BYTE				KEY				= 0x80;		//入力されているかを判断するビット位置

		LPDIRECTINPUT8			pDxInput_		= nullptr;	//DxInput Ver8型のポインタ

		//キーボード関連
		LPDIRECTINPUTDEVICE8	pKeyDevice_		= nullptr;	//キーボードデバイス
		BYTE					keyState_[256];				//キーボードの状態（押されているか）を入れる
		BYTE					prevKeyState_[256];			//前フレームでの各キーの状態

		//マウス関連
		LPDIRECTINPUTDEVICE8	pMouseDevice_	= nullptr;	//マウスデバイス
		DIMOUSESTATE			mouseState_;				//マウスの状態（押されているか）を入れる
		DIMOUSESTATE			prevMouseState_;			//前フレームでの各マウスの状態
		XMVECTOR				mousePosition_;				//マウスの位置

		//コントローラー関連
		XINPUT_STATE			controllerState_;			//コントローラーの状態（押されているか）を入れる
		XINPUT_STATE			prevControllerState_;		//前フレームでの各コントローラーの状態
	}

	//初期化
	HRESULT Initialize(const HWND& hWnd)
	{
		//DirectInputオブジェクトを作成
		if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDxInput_, nullptr)))
		{
			MessageBox(nullptr, "DirectInputオブジェクト作成に失敗しました", "エラー", MB_OK);
			return E_FAIL;
		}

		//キーボード用のデバイスオブジェクトを作成
		if (FAILED(pDxInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr)))
		{
			MessageBox(nullptr, "キーボード用のデバイスオブジェクトに失敗しました", "エラー", MB_OK);

			return E_FAIL;
		}
		pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);									//デバイスをキーボードに指定
		pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);	//優先度を設定

		//マウス用のデバイスオブジェクトを作成
		if (FAILED(pDxInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr)))
		{
			MessageBox(nullptr, "マウス用のデバイスオブジェクトに失敗しました", "エラー", MB_OK);

			return E_FAIL;
		}
		pMouseDevice_->SetDataFormat(&c_dfDIMouse);										//デバイスをマウスに指定
		pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);//優先度を設定

		return S_OK;

	}

	//更新
	void Update()
	{
		//キーボードの状態の更新
		pKeyDevice_->Acquire();														//キーボード取得
		memcpy(prevKeyState_, keyState_, sizeof(keyState_));						//keyState_ -> prevKeyState_に入れる
		pKeyDevice_->GetDeviceState(sizeof(keyState_), &keyState_);					//キーボードの今の全キーの状態を入れる

		//マウスの状態の更新
		pMouseDevice_->Acquire();													//マウス取得
		memcpy(&prevMouseState_, &mouseState_, sizeof(mouseState_));				//mouseState_ -> prevMouseState_に入れる
		pMouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);			//マウスの今の状態を入れる

		//コントローラーの状態を取得
		memcpy(&prevControllerState_, &controllerState_, sizeof(controllerState_));	//controllerState_ -> prevControllerState_に入れる
		XInputGetState(0, &controllerState_);										//コントローラーの今の状態を入れる
	}

	//キーが押されているかを調べる
	bool IsKey(int keyCode)
	{
		//押されてたらtrueを返す
		if (keyState_[keyCode] & KEY) return true;

		return false;
	}

	//キーが今押されたかを調べる
	bool IsKeyDown(int keyCode)
	{
		//今は押してて、前回は押してないならtrueを返す
		if (IsKey(keyCode) && !(prevKeyState_[keyCode] & KEY)) return true;

		return false;
	}

	//キーが今離されたかを調べる
	bool IsKeyUp(int keyCode)
	{
		//今は押してなくて、前回は押してるならtrueを返す
		if (!IsKey(keyCode) && prevKeyState_[keyCode] & KEY)	return true;

		return false;
	}

	//マウスが押されているかを調べる
	bool IsMouseButton(int buttonCode)
	{
		//押されていたらtrueを返す
		if (mouseState_.rgbButtons[buttonCode] & KEY) return true;

		return false;
	}

	//マウスが今押されたかを調べる
	bool IsMouseButtonDown(int buttonCode)
	{
		//今は押してて、前回は押してないければtrueを返す
		if (IsMouseButton(buttonCode) && !(prevMouseState_.rgbButtons[buttonCode] & KEY)) return true;
		
		return false;
	}

	//マウスが今離されたかを調べる
	bool IsMouseButtonUp(int buttonCode)
	{
		//今は押してなくて、前回は押してるならtrueを返す
		if (!IsMouseButton(buttonCode) && prevMouseState_.rgbButtons[buttonCode] & KEY) return true;
		
			return false;
	}

	//マウスの位置を入れる
	void SetMousePosition(int x, int y)
	{
		mousePosition_ = DirectX::XMVectorSet((float)x, (float)y, 0, 0);
	}

	//マウスの位置情報を取得
	XMVECTOR GetMousePosition()
	{
		return mousePosition_;
	}

	//マウスの移動量を取得
	XMVECTOR GetMouseMove()
	{
		return DirectX::XMVectorSet((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ, 0);
	}

	//ゲームパッドが押されているかを調べる
	bool IsPadButton(int buttonCode)
	{
		//押されているならtrueを返す
		if (controllerState_.Gamepad.wButtons & buttonCode) return true;
		
		return false;
	}

	//ゲームパッドが今押されたかを調べる
	bool IsPadButtonDown(int buttonCode)
	{
		//今は押してて、前回は押してないならtrueを返す
		if (IsPadButton(buttonCode) && !(prevControllerState_.Gamepad.wButtons & buttonCode)) return true;
		
		return false;
	}

	//ゲームパッドが今離されたかを調べる
	bool IsPadButtonUp(int buttonCode)
	{
		//今は押してなくて、前回は押してるならtrueを返す
		if (!IsPadButton(buttonCode) && prevControllerState_.Gamepad.wButtons & buttonCode) return true;
		
		return false;
	}

	//コントローラスティックの傾きの比率を取得
	float GetAnalogValue(int analog, int max, int deadZone)
	{
		float result = (float)analog;	//コントローラスティックの傾き

		//傾きがdeadZone未満なら傾きとして捉えない
		if (fabs(result) < deadZone)	return 0;

		//傾きの方向に合わせて傾きを-1~1で調整
		if (result > 0)	result = (result - deadZone) / (max - deadZone);	// 0から1で傾きを入れる
		else			result = (result + deadZone) / (max - deadZone);	//-1から0で傾きを入れる

		return result;
	}

	//左コントローラスティックの傾きを取得
	XMVECTOR GetPadStickL()
	{
		float x = GetAnalogValue(controllerState_.Gamepad.sThumbLX, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_.Gamepad.sThumbLY, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		return DirectX::XMVectorSet(x, y, 0, 0);
	}

	//右コントローラスティックの傾きを取得
	XMVECTOR GetPadStickR()
	{
		float x = GetAnalogValue(controllerState_.Gamepad.sThumbRX, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		float y = GetAnalogValue(controllerState_.Gamepad.sThumbRY, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
		return DirectX::XMVectorSet(x, y, 0, 0);
	}

	//左トリガーボタンの傾きを取得
	float GetPadTriggerL()
	{
		return GetAnalogValue(controllerState_.Gamepad.bLeftTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	//右トリガーボタンの傾きを取得
	float GetPadTriggerR()
	{
		return GetAnalogValue(controllerState_.Gamepad.bRightTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	}

	//振動エフェクト
	void SetPadVibration(float left, float right)
	{
		const int			MAX_VIBRATION = 65535;					//モーターの最大値
		XINPUT_VIBRATION	vibration;								//モーターの強さ

		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));			//初期化
		vibration.wLeftMotorSpeed	= (int)left * MAX_VIBRATION;	//左モーターの強さ
		vibration.wRightMotorSpeed	= (int)right * MAX_VIBRATION;	//右モーターの強さ
		XInputSetState(0, &vibration);								//入力
	}

	//解放
	void Release()
	{
		SAFE_RELEASE(pMouseDevice_);
		SAFE_RELEASE(pKeyDevice_);
		SAFE_RELEASE(pDxInput_);
	}
}

