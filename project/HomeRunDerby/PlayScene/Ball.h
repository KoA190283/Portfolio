#pragma once
#include "../Engine/IGameObject.h"
#include "Umpire.h"

const float		CURVE_AMOUNT_ADJUSTMENT = 0.00001f;		//変化球の変化量を調整するための値

//ボールの管理
class Ball : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMVECTOR		movement_;					//移動量
	XMVECTOR		rotationAxis_;				//回転軸
	float			rotation_;					//回転量
	int				hClliderBall_;				//ボールの当たり判定のハンドル

public:

	//コンストラクタ
	Ball(IGameObject* parent);

	//初期化
	void Initialize() override;

	//ボールの状態を設定
	//引数１：move			移動ベクトル
	//引数２：rotateAxis	回転軸ベクトル
	//引数３：rotate		回転数
	void SetPitchState(const XMVECTOR & move, const XMVECTOR& rotateAxis, float rotate);

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;

	//ピッチャーに投げられたボールの移動処理
	void MovePitchedBall();

	//打球の移動処理
	void MoveHitBall();

	//回転処理
	void Rotate();

	//空気抵抗力の取得
	//戻値：空気抵抗
	float GetResistance() const;			

	//重力の取得
	//戻値：重力
	float GetGravity() const;

	//マグヌスエフェクトのベクトルを取得
	//戻値：マグヌスエフェクトのベクトル
	XMVECTOR GetMagnusEffect() const;

	//他オブジェクトと衝突した時の処理
	//引数１：pTarget		当たったオブジェクト
	//引数２：pData			衝突時のデータ
	void OnCollision(IGameObject* pTarget, CollisionData *pData) override;

	//衝突時の物理計算
	//引数１：targetName	衝突したオブジェクトの名前
	//引数２：pData			衝突時のデータ
	void CalculationCollisionPhysics(const std::string& targetName, CollisionData *pData);

	//衝突面の法線の逆方向に働く運動の吸収ベクトルを取得
	//引数１：normal		衝突面の法線ベクトル
	//引数２：targetName	衝突したオブジェクトの名前
	//戻値　：吸収ベクトル
	XMVECTOR GetAbsorption(const XMVECTOR& normal, const std::string& targetName) const;

	//移動方向に働く摩擦力を取得
	//引数１：normal	衝突面の法線ベクトル
	//引数２：targetName	衝突したオブジェクトの名前
	//引数３：friction	摩擦力の出力
	//戻値　：摩擦ベクトル
	XMVECTOR GetFriction(const XMVECTOR& normal, const std::string& targetName, float* friction) const;

	//移動速度に応じた回転数の取得
	//引数：normal	衝突面の法線ベクトル
	//戻値：回転数
	float GetRotationSpeedAfterCollision(const XMVECTOR& normal) const;

	//移動方向に応じた回転軸の取得
	//引数：normal		衝突面の法線ベクトル
	//戻値：回転軸ベクトル
	XMVECTOR GetRotateAxis(const XMVECTOR& normal) const;
};