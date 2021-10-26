#include "Camera.h"

#include "Global.h"

namespace Camera
{
	//変数
	namespace
	{
		XMVECTOR position_;		//カメラの位置（視点）
		XMVECTOR target_;		//見る位置（焦点）
		XMMATRIX viewMatrix_;	//ビュー行列
		XMMATRIX projMatrix_;	//プロジェクション行列
	}

	//初期化
	void Initialize()
	{
		position_	= DirectX::XMVectorSet(0, 0, -10, 0);																								//カメラの位置
		target_		= DirectX::XMVectorSet(0, 0, 0, 0);																									//カメラの焦点
		projMatrix_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)Global::WINDOW_WIDTH / (float)Global::WINDOW_HEIGHT, 0.1f, 200.0f);	//プロジェクション行列
	}
	
	//更新
	void Update()
	{
		//ビュー行列の作成
		viewMatrix_ = DirectX::XMMatrixLookAtLH(position_, target_, DirectX::XMVectorSet(0, 1, 0, 0));
	}

	//位置を設定
	void SetPosition(const XMVECTOR &position)
	{
		position_ = position;
	}

	//float引数のSetPosition
	void SetPosition(float x, float y, float z)
	{
		SetPosition(DirectX::XMVectorSet(x, y, z, 0));
	}

	//焦点を設定
	void SetTarget(const XMVECTOR &target)
	{
		target_ = target;
	}

	//float引数のSetTarget
	void SetTarget(float x, float y, float z)
	{
		SetTarget(DirectX::XMVectorSet(x, y, z, 0));
	}

	//ビュー行列を取得
	XMMATRIX GetViewMatrix()
	{
		return viewMatrix_;
	}

	//プロジェクション行列を取得
	XMMATRIX GetProjectionMatrix()
	{
		return projMatrix_;
	}

	//位置の取得
	XMVECTOR GetPosition()
	{
		return position_;
	}
}