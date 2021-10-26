#include "../Engine/BoxCollider.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "Ball.h"
#include "Batter.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "StrikeZone.h"

using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

//コンストラクタ
StrikeZone::StrikeZone(IGameObject* parent)
	: IGameObject(parent, "StrikeZone")
{
}

//初期化
void StrikeZone::Initialize()
{
	//transform_の設定
	transform_.position_	= ExternalDataManager::GetStrikeZoneCenter();	//中心位置
	transform_.scale_.vecX	= ExternalDataManager::GetStrikeZoneWidth();	//横幅
	transform_.scale_.vecY	= ExternalDataManager::GetStrikeZoneHeight();	//高さ
	transform_.Calclation();

	//モデルデータのロード
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathStrikeZoneModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);

	//当たり判定の設定追加
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;												//オブジェクトから当たり判定へのベクトル
	BoxCollider*	newCollider					= new BoxCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//新しい当たり判定
	AddCollider(newCollider);
}

//更新
void StrikeZone::Update()
{
	//マウスの左ボタンを押されたら押された時の処理を呼ぶ
	if (Input::IsMouseButtonDown(0))	Pressed();
}

//描画
void StrikeZone::Draw()
{
	//描画するモデルの状態を設定して描画
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathStrikeZoneModel());	//ストライクゾーンのモデルハンドル
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE, Direct3D::SHADER_TYPE::NO_SHADE_3D);
	
	//的になるボールの描画
	DrawTagetBall();
}

//解放
void StrikeZone::Release()
{
}

//他オブジェクトと衝突した時の処理
void StrikeZone::OnCollision(IGameObject* pTarget, CollisionData* pData)
{
	//相手の更新が有効でなければ何もしない
	if (!pTarget->IsActive())	return;

	//更新を止める
	Inactivate();

	//描画を止める
	Invisible();
}

//押された（クリックされた）ときの処理
void StrikeZone::Pressed()
{
	//マウスの位置がストライクゾーンと重なっているかを確認して
	//重なっていたら重なった位置に向かってバッターをスイングさせる

	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;

	//ビューポート行列の作成
	float w = Global::WINDOW_WIDTH / 2.0f;	//ウィンドウ幅
	float h = Global::WINDOW_HEIGHT / 2.0f;	//ウィンドウ高さ

	const XMMATRIX VIEW_PORT = {			//ビューポート行列
		w, 0, 0, 0,
		0,-h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
	};

	//マウスの位置をワールド座標に変換するための行列を準備
	XMMATRIX inverseMatrix;																				//マウスの位置を座標に変換するための行列
	inverseMatrix =  XMMatrixInverse(nullptr, VIEW_PORT);												//ビューポート行列
	inverseMatrix *= XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());							//プロジェクション行列
	inverseMatrix *= XMMatrixInverse(nullptr, Camera::GetViewMatrix());									//ビュー行列

	//マウスのワールド座標を作成
	XMVECTOR mousePositionFront, mousePositionBack;														//マウスの座標
	mousePositionFront		= Input::GetMousePosition();												//マウスの座標（手前）
	mousePositionBack		= Input::GetMousePosition();												//マウスの座標（奥）
	mousePositionFront.vecZ = 0.0f;																		//ｚ軸の移動を０（手前）に
	mousePositionBack.vecZ	= 1.0f;																		//ｚ軸の移動を１（奥）に

	mousePositionFront	= XMVector3TransformCoord(mousePositionFront, inverseMatrix);					//変換行列で手前のマウス座標を変換
	mousePositionBack	= XMVector3TransformCoord(mousePositionBack, inverseMatrix);					//変換行列で奥のマウス座標を変換

	//レイキャストに使用する構造体の準備	
	RayCastData data;																					//レイキャストで使用するデータ
	data.start		= mousePositionFront;																//レイの開始位置
	data.direction	= DirectX::XMVector3Normalize(mousePositionBack - mousePositionFront);				//レイの方向
	
	//レイキャスト
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathStrikeZoneModel());		//ストライクゾーンのモデルハンドル
	Model::SetTransform(MODEL_HANDLE, transform_);														//判定するオブジェクトのtransform_情報を設定
	Model::RayCast(MODEL_HANDLE, &data);																//レイキャスト

	//レイが当たっていたらバッターのスイング関数を呼ぶ
	if (data.isHit)
	{
		//クリックされた位置を取得する
		XMVECTOR clickPosistion = data.direction * data.distance + data.start;							//クリックされた位置
		clickPosistion			= XMVector3TransformCoord(clickPosistion, transform_.GetWorldMatrix());	//ワールド行列で変換
		clickPosistion.vecZ		= 0;																	//Z方向のずれをなくす

		//バッターのスイング関数を呼ぶ
		Batter*	 pBatter = (Batter*)(GetParent()->FindChildObject("Batter"));							//バッターのポインタ
		pBatter->Swing(clickPosistion);																	//スイング関数の呼び出し
	}
}

//的になるボールの描画
void StrikeZone::DrawTagetBall()
{
	using DirectX::XMVector3Normalize;
	using DirectX::XMVector3Cross;
	using DirectX::operator-=;
	using DirectX::operator*=;
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;

	//プレーが開始されていなければ描画しない
	PlayScene*	pPlayScene	= (PlayScene*)pParent_;								//プレイシーンのポインタ
	if (!pPlayScene->IsInPlay())	return;

	//ボールの更新が有効でなければ何もしない
	Ball*		pBall		= (Ball*)(pParent_->FindChildObject("Ball"));		//バッターのポインタ
	if (!pBall->IsActive())	return;

	Pitcher*	pPitcher	= (Pitcher*)(pParent_->FindChildObject("Pitcher"));	//ピッチャーのポインタ
	
	//的用ボールを描画する位置を決める
	const XMVECTOR	STRIKE_ZONE_CENTER_POSITION	= ExternalDataManager::GetStrikeZoneCenter();														//ストライクゾーンの中心位置
	XMVECTOR		drawPosition				= STRIKE_ZONE_CENTER_POSITION + pPitcher->GetAimPosition();											//描画位置

	//選択されている球種がストレート以外ならストライクゾーン上の狙っている場所から変化方向の逆方向に描画位置を動かす	
	const int		BALL_TYPE					= pPitcher->GetBallType();																			//選択されている球種
	const int		FAST_BALL					= 0;																								//ファストボール（ストレート）
	if (BALL_TYPE != FAST_BALL)
	{
		//描画上のボールの変化を設定する
		const float		DEGREE_MAX					= 360.f;																												//最大角度
		const float		CURVE_ADJUSTMENT			= 0.0000002f;																											//変化量を調整するための値
		const float		ROTATE						= ExternalDataManager::GetRotate(BALL_TYPE) * DEGREE_MAX;																//回転量
		const XMVECTOR	ROTATE_AXIS					= ExternalDataManager::GetRotateAxis(BALL_TYPE);																		//回転軸
		const XMVECTOR	PITCHER_TO_HOME_DIRECTION	= XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());		//ピッチャーからホームへの単位ベクトル
		const XMVECTOR	CURVE_DIRECTION				= XMVector3Normalize(XMVector3Cross(ROTATE_AXIS, PITCHER_TO_HOME_DIRECTION));											//曲がる方向への単位ベクトル
		const XMVECTOR	DRAW_CURVE_MAX				= CURVE_DIRECTION * ROTATE * CURVE_ADJUSTMENT;																			//描画上の最大変化ベクトル
		const float		TO_BALL_DISTANCE			= (pBall->GetPosition() - STRIKE_ZONE_CENTER_POSITION).vecZ;															//ストライクゾーンとボールのZ方向の距離
		const float		TO_PITCHER_DISTANCE			= (pPitcher->GetReleasePosition() - STRIKE_ZONE_CENTER_POSITION).vecZ;													//ストライクゾーンとリリースポイントのZ方向の距離
		const float		CURVE_PERCENTAGE			= TO_BALL_DISTANCE / TO_PITCHER_DISTANCE;																				//変化量の割合

		//変化方向と逆方向に割合だけピッチャーが狙っている場所からずらす
		drawPosition -= DRAW_CURVE_MAX * CURVE_PERCENTAGE;											
	}

	//的用ボールの回転状態を決める
	static float	rotation			= 0.f;																						//回転量
	const XMVECTOR	ROTATE_AXIS			= ExternalDataManager::GetRotateAxis(BALL_TYPE);											//回転軸
	const XMVECTOR  QUATERNION			= DirectX::XMQuaternionRotationAxis(ROTATE_AXIS, DirectX::XMConvertToRadians(rotation));	//回転状態
	rotation += 40.f;																												//1f分の回転量を加算する

	//的用ボールを描画する状態を作成する
	Transform transform;
	transform.position_		=	drawPosition;							//描画位置
	transform.scale_		*=	ExternalDataManager::GetBallRadius();	//拡大率
	transform.quaternion_	=	QUATERNION;								//回転状態

	//作成した情報を使用して描画する
	const int MODEL_HANDLE	=	Model::GetHandle(ExternalDataManager::GetFilePathBallModel());	//ボールのモデルハンドル
	Model::SetTransform(MODEL_HANDLE, transform);
	Model::Draw(MODEL_HANDLE, Direct3D::SHADER_TYPE::NO_SHADE_3D);
}
