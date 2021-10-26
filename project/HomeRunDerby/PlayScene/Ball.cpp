#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Transform.h"
#include "../ExternalDataManager.h"
#include "Ball.h"
#include "Batter.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "StrikeZone.h"

using DirectX::XMVECTOR;
using DirectX::operator+=;
using DirectX::operator*=;
using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator*;

//コンストラクタ
Ball::Ball(IGameObject* parent)
	: IGameObject(parent, "Ball"), rotation_(0), hClliderBall_(0)
{
	movement_		= DirectX::XMVectorSet(0, 0, 0, 0);
	rotationAxis_	= DirectX::XMVectorSet(0, 0, 0, 1);
}

//初期化
void Ball::Initialize()
{
	//モデルデータのロード
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathBallModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);

	//当たり判定の設定追加
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;													//オブジェクトから当たり判定へのベクトル
	SphereCollider* newCollider					= new SphereCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//新しい当たり判定
	newCollider->Continuous();																							//当たり判定すり抜け防止
	AddCollider(newCollider);																							//ゲームオブジェクトに当たり判定を追加

	//更新を止める
	Inactivate();

	//描画を止める
	Invisible();
}

//更新
void Ball::Update()
{
	//ボールが打球の状態であれば打球用の更新を行う
	if (((PlayScene*)pParent_)->IsHitBall())	MoveHitBall();

	//ピッチャーに投げられてから状態が変わっていなければ投げられたボールの移動処理を行う
	else										MovePitchedBall();

	//回転させる
	Rotate();
}

//描画
void Ball::Draw()
{
	//描画するモデルの状態を設定して描画
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathBallModel());	//ボールのモデルハンドル
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE);
}

//解放
void Ball::Release()
{
}

//打球の移動処理
void Ball::MoveHitBall()
{
	XMVECTOR moveDiff		=  DirectX::g_XMZero;		//前フレームの移動ベクトルとの差分
	moveDiff.vecY			-= GetGravity();			//重力
	moveDiff				+= GetMagnusEffect();		//マグヌス効果
	moveDiff				*= 1.f - GetResistance();	//空気抵抗

	movement_ += moveDiff;								//移動ベクトルを変化させる
	transform_.position_	+= movement_;				//位置を変更
}

//ピッチャーに投げられたボールの移動処理
void Ball::MovePitchedBall()
{
	Pitcher* pPitcher = (Pitcher*)pParent_->FindChildObject("Pitcher");	//ピッチャーのゲームオブジェクト

	const int		BALL_TYPE	= pPitcher->GetBallType();				//選択されている球種
	const int		FAST_BALL	= 0;									//ファストボール（ストレート）

	//選択されている球種がファストボール以外であれば回転量と回転軸方向に合わせて移動ベクトルに変化を与える
	if (BALL_TYPE != FAST_BALL)
	{
		const XMVECTOR	PITCH_DIRECTION		= DirectX::XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());	//ピッチャーからホームへの向きベクトル
		const XMVECTOR	CURVE_DIRECTION		= DirectX::XMVector3Cross(rotationAxis_, PITCH_DIRECTION);																		//変化する方向
		const float		CURVE_AMOUNT		= rotation_ * CURVE_AMOUNT_ADJUSTMENT;																							//変化量

		movement_ += CURVE_DIRECTION * CURVE_AMOUNT * ((PlayScene*)pParent_)->GetGameSpeedAcceleration();
	}

	transform_.position_ += movement_;			//位置を変更
}

//回転処理
void Ball::Rotate()
{
	if (DirectX::XMVector3Equal(rotationAxis_, DirectX::g_XMZero)) return;																//回転軸が無ければ何もしない

	const float		DEGREE_MAX		= 360.f;																							//最大角度
	const float		ROTATION_DEGREE	= rotation_ * DEGREE_MAX;																			//回転量
	const XMVECTOR  QUATERNION		= DirectX::XMQuaternionRotationAxis(rotationAxis_, DirectX::XMConvertToRadians(ROTATION_DEGREE));	//フレームでの回転クォータニオン
	
	transform_.quaternion_			= DirectX::XMQuaternionMultiply(transform_.quaternion_, QUATERNION);								//前フレームまでの回転と次フレームの回転を合成
}

//ボールの状態を設定
void Ball::SetPitchState(const XMVECTOR& move, const XMVECTOR& rotateAxis, float rotate)
{
	const float		ACCELERATION = ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//ゲーム内の加速度

	//オブジェクトの状態を設定
	transform_.scale_		= DirectX::g_XMOne * ExternalDataManager::GetBallRadius();
	transform_.quaternion_	= DirectX::XMVectorSet(0, 0, 0, 1);
	movement_				= move * ACCELERATION;
	rotationAxis_			= rotateAxis;
	rotation_				= rotate * ACCELERATION;

	//当たり判定の設定
	SetColliderSize(hClliderBall_, transform_.scale_);
}

//他オブジェクトと衝突した時の処理
void Ball::OnCollision(IGameObject* pTarget, CollisionData* pData)
{
	if (!IsActive())	return;													//更新してなければ何もしない

	const std::string	TARGET_NAME				= pTarget->GetName();			//衝突したオブジェクトの名前
	const XMVECTOR		COLLISION_POSITION_BALL = pData->positionList.front();	//衝突時のボールの位置
	
	//衝突したことをシーンオブジェクトに伝える
	PlayScene* pPlayScene	= (PlayScene*)pParent_;								//シーンオブジェクト
	pPlayScene->CollisionBall(COLLISION_POSITION_BALL, pTarget->GetName());

	//衝突したオブジェクトがストライクゾーンであれば物理的な変化はないので以降の処理は行わない
	if (TARGET_NAME == "StrikeZone")	return;

	//衝突時の物理計算
	CalculationCollisionPhysics(TARGET_NAME, pData);
}

//空気抵抗の取得
float Ball::GetResistance() const
{
	//移動が無ければ0を返す
	const bool IS_STOP = DirectX::XMVector3Equal(movement_, DirectX::g_XMZero);			//止まっているか
	if (IS_STOP) return 0.f;															

	const float ACCELERATION	= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//ゲーム内の加速度
	const float	BALL_RADIUS		= ExternalDataManager::GetBallRadius();					//ボールの半径(m)
	const float DENSITY			= 1.1681f;												//空気の密度(kg/m^3)
	const float VISCOSITY		= 1.8f * powf(0.1f,5) / Global::GAME_FPS * ACCELERATION;//空気の粘度(Pa/f)
	const float SPEED			= DirectX::XMVector3Length(movement_).vecX;				//速度(m/f)
	const float AREA			= powf(BALL_RADIUS, 2) * DirectX::XM_PI;				//面積(m^2)
	const float REYNOLDS		= DENSITY * SPEED * BALL_RADIUS * 2 / VISCOSITY;		//レイノルズ数

	float	coefficient;																//抗力係数
	if		(REYNOLDS < 2)		coefficient = 24.f / REYNOLDS;							//層流域
	else if	(REYNOLDS < 500)	coefficient = 10.f / sqrtf(REYNOLDS);					//中間域
	else						coefficient = 0.44f;									//乱流域

	return  0.5f * DENSITY * powf(SPEED, 2) * AREA * coefficient;						//抗力
}

//重力の取得
float Ball::GetGravity() const
{
	const float TIME_ACCELERATION		= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//ゲーム内の加速度
	const float GRAVITY_ACCELERATION	= 9.80665f;												//重力加速度(m/s^2)
	const float TIME					= 1.f / Global::GAME_FPS * TIME_ACCELERATION;			//移動時間(s)
	
	return 0.5f * GRAVITY_ACCELERATION * powf(TIME, 2);											//1f当たりの重力
}

//マグヌス効果の取得
XMVECTOR Ball::GetMagnusEffect() const
{
	//移動が無ければ0ベクトルを返す
	const bool IS_STOP = DirectX::XMVector3Equal(movement_, DirectX::g_XMZero);				//止まっているか
	if (IS_STOP)		return DirectX::g_XMZero;					

	//回転軸が無ければ0ベクトルを返す
	const bool EXISTS = DirectX::XMVector3Equal(rotationAxis_, DirectX::g_XMZero);
	if (EXISTS)	return DirectX::g_XMZero;

	const XMVECTOR	EFFECT_DIRECTION = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rotationAxis_, movement_));			//回転軸と運動ベクトルの外積の逆ベクトル
	const float		DENSITY			 = 1.1681f;																					//空気の密度(kg/m^3)
	const float		DIAMETER		 = ExternalDataManager::GetBallRadius() * 2.f;												//ボールの直径(m)
	const float		VELOCITY		 = DirectX::XMVector3Length(movement_).vecX;												//速度(m/f)

	const float		LIFT			 = 1.f / 8.f * powf(DirectX::XM_PI, 2) * DENSITY * powf(DIAMETER, 3) * VELOCITY * rotation_;//揚力

	return EFFECT_DIRECTION * LIFT;																								//1f当たりのマグヌス効果
}

//衝突時の物理計算
void Ball::CalculationCollisionPhysics(const std::string& targetName, CollisionData* pData)
{
	//衝突後の移動ベクトル
	float				friction				= 0.f;														//摩擦力
	const XMVECTOR		COLLISION_POSITION_BALL = pData->positionList.front();								//衝突時のボールの位置
	const XMVECTOR		NORMAL_DIRECTION		= DirectX::XMVector3Normalize(-pData->direction);			//衝突面の法線ベクトル
	const XMVECTOR		FRICTION				= GetFriction(NORMAL_DIRECTION, targetName, &friction);		//摩擦
	const XMVECTOR		REFLECT					= DirectX::XMVector3Reflect(movement_, NORMAL_DIRECTION);	//ボールの反射ベクトル
	const XMVECTOR		ABSORPTION				= GetAbsorption(NORMAL_DIRECTION, targetName);				//吸収される力

	XMVECTOR			newMovement				= REFLECT + ABSORPTION + FRICTION;							//衝突判定後の移動ベクトル

	//衝突したオブジェクト名がバッターならスイングの影響を反映する
	if (targetName == "Batter")
	{
		Batter* pBatter = (Batter*)pParent_->FindChildObject("Batter");										//バッターのオブジェクト

		const XMVECTOR	SWING_VECTOR		= pBatter->GetSwingVector(COLLISION_POSITION_BALL);				//ボールに対するスイングのベクトル
		const XMVECTOR	SWING_DRECTION		= DirectX::XMVector3Normalize(SWING_VECTOR);					//スイングの向きベクトル
		const float		DOT_SWING_NORMAL	= DirectX::XMVector3Dot(SWING_DRECTION, NORMAL_DIRECTION).vecX;	//スイングの向きベクトルと衝突面の法線ベクトルの内角
		const float		PERCENTAGE			= 0.5f + 0.5f * DOT_SWING_NORMAL;								//スイングの影響力
		const float		ACCELERATION		= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();			//ゲーム内の加速度
		
		//スイングのベクトルを衝突面の法線ベクトルとの角度とゲーム内の加速度に合わせて大きさを変えてボールの移動ベクトルに反映する
		newMovement += SWING_VECTOR * PERCENTAGE * ACCELERATION;
	}

	//静止判定
	const bool IS_STOP = DirectX::XMVector3Length(newMovement).vecX < friction;				//運動の力と摩擦力どちらが大きいか
	if (IS_STOP) newMovement = DirectX::g_XMZero;											//静止していたら運動量を0にする

	//地面へのめり込み防止
	if (targetName == "Field")																//衝突したオブジェクトがグラウンドなら
		if (newMovement.vecY < 0.f) newMovement.vecY = 0.f;									//グラウンドよりも下にはいかない

	//衝突後の位置と移動ベクトルを設定
	const float	MAX_LENGTH			= DirectX::XMVector3Length(movement_).vecX;											//フレームの移動距離
	const float	SURPLUS_DISTANCE	= DirectX::XMVector3Length(transform_.position_ - COLLISION_POSITION_BALL).vecX;	//衝突後の余剰距離
	const float	PERCENTAGE			= SURPLUS_DISTANCE / MAX_LENGTH;													//衝突フレームの衝突前後の移動比率
	transform_.position_			= COLLISION_POSITION_BALL + (newMovement * PERCENTAGE);								//衝突フレームの位置
	movement_						= newMovement;																		//衝突後の移動ベクトル

	//衝突後の回転を設定
	rotationAxis_	= GetRotateAxis(NORMAL_DIRECTION);							//回転軸
	rotation_		= GetRotationSpeedAfterCollision(NORMAL_DIRECTION);			//回転数
}

//力の吸収ベクトル（弾性の逆ベクトル）の取得
XMVECTOR Ball::GetAbsorption(const XMVECTOR& normal, const std::string& targetName) const
{
	const XMVECTOR	EFFECT_DIRECTION		= DirectX::XMVector3Normalize(-normal);								//衝突面の法線ベクトル
	
	const float		DOT_EFFECT_MOVEMENT		= DirectX::XMVector3Dot(EFFECT_DIRECTION, movement_).vecX;			//効果が発生する向きベクトルと運動ベクトルの内積

	const float		RESTITUTION_BALL		= ExternalDataManager::GetCoefficientRestitution(GetName());		//ボールの反発係数
	const float		RESTITUTION_TARGET		= ExternalDataManager::GetCoefficientRestitution(targetName);		//衝突したオブジェクトの反発係数
	const float		COEFFICIENT				= 1.f - RESTITUTION_BALL * RESTITUTION_TARGET;						//反発係数

	return EFFECT_DIRECTION * DOT_EFFECT_MOVEMENT * COEFFICIENT;												//力の吸収ベクトル
}

//摩擦力の取得
XMVECTOR Ball::GetFriction(const XMVECTOR& normal, const std::string& targetName, float* friction) const
{ 
	using DirectX::XMVector3Normalize;
	using DirectX::XMVector3Cross;
	using DirectX::XMVector3Length;

	const XMVECTOR	MOVEMENT_DIRECTION				= XMVector3Normalize(movement_);												//移動ベクトルの向きベクトル
	const XMVECTOR	NORMAL_DIRECTION				= XMVector3Normalize(normal);													//衝突面の法線ベクトル
	const XMVECTOR	ROTATINOAL_FRICTION_DIRECTION	= XMVector3Normalize(XMVector3Cross(NORMAL_DIRECTION, rotationAxis_));			//回転による摩擦力が働く方向
	const XMVECTOR	CROSS_NORMAL_MOVEMENT			= XMVector3Cross(NORMAL_DIRECTION, MOVEMENT_DIRECTION);							//移動ベクトルと衝突面法線の外積
	const XMVECTOR	MOVEMENT_FRICTION_DIRECTION		= XMVector3Normalize(XMVector3Cross(NORMAL_DIRECTION, CROSS_NORMAL_MOVEMENT));	//運動による摩擦力が働く方向
	const float		BALL_RADIUS						= ExternalDataManager::GetBallRadius();											//ボールの半径(m)
	const float		BALL_WEIGHT						= ExternalDataManager::GetBallWeight();											//ボールの重さ(kg)
	const float		DEGREE_MAX						= 360.f;																		//最大角度(deg)
	const float		DOT								= DirectX::XMVector3Dot(NORMAL_DIRECTION, -MOVEMENT_DIRECTION).vecX;			//移動ベクトルと衝突面の法線ベクトルの内積
	const float		VELOCITY						= XMVector3Length(movement_).vecX;												//ボールの速度(m/f)
	const float		COEFFICIENT_BALL				= ExternalDataManager::GetCoefficientFriction(GetName());						//ボールの動摩擦係数
	const float		COEFFICIENT_TERGET				= ExternalDataManager::GetCoefficientFriction(targetName);						//衝突したオブジェクトの動摩擦係数
	const float		COEFFICIENT						= COEFFICIENT_BALL * COEFFICIENT_TERGET;										//動摩擦係数
	const float		ANGULAR_VELOCITY				= 2.f * DirectX::XM_PI * rotation_;												//角速度(degree/f)
	const float		NORMAL_FORCE					= BALL_WEIGHT * (1.f + VELOCITY * DOT);											//垂直抗力
	const float		FRICTION_FORCE					= COEFFICIENT * NORMAL_FORCE;													//動摩擦力
	const float		INERTIA							= NORMAL_FORCE * powf(BALL_RADIUS, 2);											//慣性モーメント
	const float		RORTATINOAL_FORCE				= 0.5f * INERTIA *  powf(ANGULAR_VELOCITY, 2);									//回転による運動量

	const XMVECTOR	MOVEMENT_FRICTION				= MOVEMENT_FRICTION_DIRECTION * FRICTION_FORCE;									//移動方向に関して働く力
	const XMVECTOR	RORTATINOAL_FRICTION			= ROTATINOAL_FRICTION_DIRECTION * RORTATINOAL_FORCE * FRICTION_FORCE;			//回転方向に関して働く力

	if (friction != nullptr) *friction = FRICTION_FORCE;																			//摩擦力を出力変数に入力
	
	return MOVEMENT_FRICTION + RORTATINOAL_FRICTION;																				//摩擦力
}

//回転数の取得
float Ball::GetRotationSpeedAfterCollision(const XMVECTOR& normal) const
{
	const XMVECTOR	MOVEMENT_DIRECTION	= DirectX::XMVector3Normalize(movement_);			//移動方向
	const XMVECTOR	COLLISION_DIRECTION = DirectX::XMVector3Normalize(normal);				//衝突面の法線ベクトル
	const float		BALL_RADIUS			= ExternalDataManager::GetBallRadius();				//ボールの半径(m)
	const float		CIRCUMFERENCE		= 2.f * DirectX::XM_PI * BALL_RADIUS;				//円周(m)
	const float		VELOCITY			= DirectX::XMVector3Length(movement_).vecX;			//速度(m/f)
	
	return VELOCITY / CIRCUMFERENCE;														//回転量(rotate/f)
}

//回転軸の取得
XMVECTOR Ball::GetRotateAxis(const XMVECTOR& normal) const
{
	return DirectX::XMVector3Normalize(DirectX::XMVector3Cross(movement_, normal));		//回転軸方向
}