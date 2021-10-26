#include "Transform.h"

#include <fstream>

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

//コンストラクタ
Transform::Transform():pParent_(nullptr)
{
	position_	  = DirectX::XMVectorSet(0, 0, 0, 0);	//位置
	quaternion_	  = DirectX::XMVectorSet(0, 0, 0, 1);	//クォータニオン
	scale_		  = DirectX::XMVectorSet(1, 1, 1, 0);	//拡大率

	matTranslate_ = DirectX::XMMatrixIdentity();		//移動行列
	matRotation_  = DirectX::XMMatrixIdentity();		//回転行列	
	matScale_	  = DirectX::XMMatrixIdentity();		//拡大行列
}

//移動行列を取得
XMMATRIX Transform::GetTranslateMatrix() const
{ 
	return matTranslate_;
}

//回転行列を取得
XMMATRIX Transform::GetRotateMatrix() const
{ 
	return matRotation_;
}			

//拡大行列を取得
XMMATRIX Transform::GetScaleMatrix() const
{
	return matScale_; 
}

//ワールド行列を取得
XMMATRIX Transform::GetWorldMatrix()
{
	//ワールド行列を作成する
	XMMATRIX worldMatrix = matScale_ * matRotation_ * matTranslate_;			//ワールド行列
	
	//親がいたら親のワールド行列をかける
	if (pParent_ != nullptr)	worldMatrix *= pParent_->GetWorldMatrix();	

	return worldMatrix;
}

//回転を設定
void Transform::SetRotate(const XMVECTOR & rotate)
{
	quaternion_ = DirectX::XMQuaternionRotationRollPitchYawFromVector(ConvertToRadian(rotate));				//オイラー角をクォータニオン回転に変換
}

//親の変形情報の設定
void Transform::SetParent(Transform * pParent)
{
	pParent_ = pParent;
}

//X軸の回転を追加
void Transform::AddRotateX(float x)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(x), 0, 0);	//オイラー角をクォータニオン回転に変換
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//オイラー角回転とクォータニオンを乗算
}

//Y軸の回転を追加
void Transform::AddRotateY(float y)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(0, DirectX::XMConvertToRadians(y), 0);	//オイラー角をクォータニオン回転に変換
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//オイラー角回転とクォータニオンを乗算
}

//Z軸の回転を追加
void Transform::AddRotateZ(float z)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(z));	//オイラー角をクォータニオン回転に変換
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//オイラー角回転とクォータニオンを乗算
}

//各行列の計算
void Transform::Calclation()
{
	//移動行列
	matTranslate_ = DirectX::XMMatrixTranslation(
		position_.vecX,
		position_.vecY,
		position_.vecZ
	);

	//回転行列
	matRotation_ = DirectX::XMMatrixRotationQuaternion(quaternion_);

	//拡大縮小行列
	matScale_ = DirectX::XMMatrixScaling(scale_.vecX, scale_.vecY, scale_.vecZ);
}

//オイラー角の各角度を度数法から弧度法に変換
XMVECTOR Transform::ConvertToRadian(const XMVECTOR& rotate) const
{
	return  DirectX::XMVectorSet(
		DirectX::XMConvertToRadians(rotate.vecX),	//x
		DirectX::XMConvertToRadians(rotate.vecY),	//y
		DirectX::XMConvertToRadians(rotate.vecZ),	//z
		0											//w
		);
}