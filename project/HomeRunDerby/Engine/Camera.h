#pragma once
#include <DirectXMath.h>

#include "Direct3D.h"

//カメラ
namespace Camera
{
	using DirectX::XMMATRIX;
	using DirectX::XMVECTOR;

	//初期化（プロジェクション行列作成）
	void Initialize();

	//更新（ビュー行列作成）
	void Update();

	//視点（カメラの位置）を設定
	//引数：position　XMVECTOR型のカメラ座標
	void SetPosition(const XMVECTOR &position);

	//視点（カメラの位置）を設定
	//引数：x　x軸のカメラ座標
	//引数：y　y軸のカメラ座標
	//引数：z　z軸のカメラ座標
	void SetPosition(float x, float y, float z);

	//焦点（見る位置）を設定
	//引数：position　XMVECTOR型のカメラ焦点
	void SetTarget(const XMVECTOR &target);

	//焦点（見る位置）を設定
	//引数：x　x軸のカメラ焦点
	//引数：y　y軸のカメラ焦点
	//引数：z　z軸のカメラ焦点
	void SetTarget(float x, float y, float z);

	//取得関数
	XMMATRIX GetViewMatrix();		//ビュー行列
	XMMATRIX GetProjectionMatrix();	//プロジェクション行列
	XMVECTOR GetPosition();			//位置
};

