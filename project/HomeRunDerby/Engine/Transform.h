#pragma once
#include <DirectXMath.h>

#define vecX m128_f32[0]	//x
#define vecY m128_f32[1]	//y
#define vecZ m128_f32[2]	//z
#define vecW m128_f32[3]	//w

//位置、向き、拡大率などを管理するクラス
class Transform
{
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMMATRIX	matTranslate_;					//移動行列
	XMMATRIX	matRotation_;					//回転行列	
	XMMATRIX	matScale_;						//拡大行列
	Transform*	pParent_;						//親オブジェクトのTransform

public:

	XMVECTOR position_;							//位置
	XMVECTOR quaternion_;						//クォータニオン回転
	XMVECTOR scale_;							//拡大率

public:

	//コンストラクタ
	Transform();

	////////////////////
	//取得関数
	XMMATRIX GetTranslateMatrix() const;		//移動行列を取得
	XMMATRIX GetRotateMatrix() const;			//回転行列を取得
	XMMATRIX GetScaleMatrix() const;			//拡大行列を取得
	XMMATRIX GetWorldMatrix();					//ワールド行列を取得

	
	////////////////////
	//設定関数
	void SetRotate(const XMVECTOR & rotate);	//回転を設定　			rotate：オイラー角
	void SetParent(Transform * pParent);		//親の変形情報の設定	pParent	親の変形情報のポインタ

	////////////////////
	//追加関数
	void AddRotateX(float x);				//X軸の回転を追加	x:x軸の回転量（度数）
	void AddRotateY(float y);				//Y軸の回転を追加	y:y軸の回転量（度数）
	void AddRotateZ(float z);				//Z軸の回転を追加	z:z軸の回転量（度数）


	//各行列の計算
	void Calclation();

private:

	//オイラー角の各角度を度数法から弧度法に変換
	//引数：rotate	度数法の角度
	//戻値：弧度法の角度
	XMVECTOR ConvertToRadian(const XMVECTOR& rotate) const;
};