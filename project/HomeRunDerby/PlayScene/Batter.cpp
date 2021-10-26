#include "../Engine/CapsuleCollider.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Math.h"
#include "../ExternalDataManager.h"
#include "Batter.h"
#include "PlayScene.h"


//コンストラクタ
Batter::Batter(IGameObject* parent)
	: IGameObject(parent, "Batter"), prevBatPosition_(DirectX::g_XMZero), nextAnimationFrame_(0), hModelDraw_(-1), hColliderBat_(-1), isSwing_(0)
{
}

//初期化
void Batter::Initialize()
{
	//モデルデータのロード
	for (int i = 0; i < (int)SWING_COURSE::MAX; i++)
	{
		const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathBatterModel((SWING_COURSE)i));
		assert(MODEL_HANDLE != Model::ERROR_CORD);
	}

	//アニメーションを最初の状態にする
	ResetAnimation();

	//ストライクゾーンの設定を行う
	SetStrikeZone();
	
	//当たり判定の設定追加
	const XMVECTOR	COLLIDER_RELATIVE_POSITIONS[2]	= { DirectX::g_XMZero,DirectX::g_XMZero };							//オブジェクトから当たり判定への初期ベクトル
	const XMVECTOR	COLLIDER_SIZE					= DirectX::g_XMOne * ExternalDataManager::GetBatRadius();			//当たり判定のサイズ
	CapsuleCollider* newCollider					= new CapsuleCollider(COLLIDER_RELATIVE_POSITIONS, COLLIDER_SIZE);	//新しい当たり判定
	hColliderBat_ = AddCollider(newCollider);																			//ゲームオブジェクトに当たり判定を追加
	SetBatColliderPosition();																							//当たり判定の位置を設定
}

//更新
void Batter::Update()
{
	//当たり判定の位置を再設定
	SetBatColliderPosition();

	if (!isSwing_)	return;		//スイングしていなければ以降の処理は行わない

	//アニメーションのフレームを進める
	nextAnimationFrame_++;

	//次フレームのアニメーションの姿勢を設定する
	SetNextAnimationPose();

	//アニメーションが終了していなければ以下の処理を行わない
	const int ANIMATION_FRAME_MAX = 60;
	if (nextAnimationFrame_ < ANIMATION_FRAME_MAX)	return;
	
	//アニメーションを初期の状態に戻す
	ResetAnimation();
}

//描画
void Batter::Draw()
{
	//描画するモデルの状態を設定して描画
	Model::SetTransform(hModelDraw_, transform_);
	Model::Draw(hModelDraw_);
}

//解放
void Batter::Release()
{
}

//オブジェクトに対してスイングが与える影響（ベクトル）を取得する
DirectX::XMVECTOR Batter::GetSwingVector(const XMVECTOR& hitPosition) const
{
	using DirectX::operator-;
	using DirectX::operator*;

	const XMVECTOR	BAT_CENTER_POSITION	= Model::GetBonePosition(hModelDraw_, "batCenter");				//バットの中心（芯）の位置
	const XMVECTOR	BAT_GRIP_POSITION	= Model::GetBonePosition(hModelDraw_, "batGrip");				//バットのグリップの位置
	const XMVECTOR	BAT_TIP_POSITION	= Model::GetBonePosition(hModelDraw_, "batTip");				//バットの先端の位置

	//スイングの向きベクトルの取得
	XMVECTOR swingDirection = DirectX::XMVector3Normalize(BAT_CENTER_POSITION - prevBatPosition_);		//スイングの向きベクトル
	{
		//左右方向へのベクトルを少し小さくする
		swingDirection.vecX *= 0.8f;

		//下向きにならないように調整
		const float ADJUST_VECTOR_Y_MIN = 0.3f;
		if (swingDirection.vecY < ADJUST_VECTOR_Y_MIN)		swingDirection.vecY = ADJUST_VECTOR_Y_MIN;

		//上を向きすぎないように調整
		const float ADJUST_VECTOR_Y_MAX = 0.6f;
		if (swingDirection.vecY > ADJUST_VECTOR_Y_MAX)		swingDirection.vecY = ADJUST_VECTOR_Y_MAX;

		//正面方向へのベクトルを少し大きくする
		swingDirection.vecZ -= 0.3f;

		//正規化して単位ベクトルにする
		swingDirection = DirectX::XMVector3Normalize(swingDirection);
	}

	//力の大きさの取得
	const float		DISTANCE_MAX				= DirectX::XMVector3Length(BAT_GRIP_POSITION - BAT_TIP_POSITION).vecX + ExternalDataManager::GetBatRadius();		//バットとオブジェクトの距離の最大値
	const float		DISTANCE_TO_HIT_POSITION	= DirectX::XMVector3Length(BAT_CENTER_POSITION - hitPosition).vecX;													//バットの芯とオブジェクトの距離
	const float		DISTANCE_PERCENTAGE			= 0.5f + 0.5f * DISTANCE_TO_HIT_POSITION / DISTANCE_MAX;															//オブジェクトに与える衝撃の割合
	const float		ADJUST_POWER_IMPACT_FRAME	= 0.05f;																											//最大値フレーム数から1フレームずれるごとにどれだけ力が落ちるか
	const float		IMPACT_PERCENTAGE			= 1.f / (1.f + ADJUST_POWER_IMPACT_FRAME * abs(nextAnimationFrame_ - ExternalDataManager::GetBatterImpactFrame()));	//次フレームの力の割合
	
	return swingDirection * ExternalDataManager::GetBatterPower() * DISTANCE_PERCENTAGE * IMPACT_PERCENTAGE;
}

//swingPositionにスイングするために必要な情報を取得して次のUpdateからスイングする
void Batter::Swing(const XMVECTOR& swingPosition)
{
	using DirectX::operator-;
	using DirectX::XMVectorSet;


	if (isSwing_) return;											//スイング中なら何もしない

	const XMVECTOR	SWING_POSITION		= swingPosition - ExternalDataManager::GetStrikeZoneCenter();	//スイング位置（ストライクゾーンの中心を原点に）
	const float		HALF				= 0.5f;															//半分
	const float		HALF_HEIGHT			= ExternalDataManager::GetStrikeZoneHeight() * HALF;			//ストライクゾーンの半分の高さ
	const float		HALF_WIDTH			= ExternalDataManager::GetStrikeZoneWidth() * HALF;				//ストライクゾーンの半分の横幅
	const int		POLYGON_VERTICES	= 3;															//三角ポリゴンに含まれる頂点数
	const int		POLYGON_COUNT		= 4;															//ストライクゾーンのポリゴン数
	const int		NEED_COURSE_MAX		= 2;															//スイングアニメーションの合成に必要なコース数
	
	const XMVECTOR CORNER_POSITION[(int)SWING_COURSE::MAX] =		//ストライクゾーンの頂点の座標		
	{
		XMVectorSet( HALF_WIDTH, HALF_HEIGHT, 0, 0),				//左上
		XMVectorSet( HALF_WIDTH,-HALF_HEIGHT, 0, 0),				//左下
		XMVectorSet(-HALF_WIDTH, HALF_HEIGHT, 0, 0),				//右上
		XMVectorSet(-HALF_WIDTH,-HALF_HEIGHT, 0, 0),				//右下
		DirectX::g_XMZero,											//中心
	};

	const SWING_COURSE INDEX[POLYGON_COUNT][POLYGON_VERTICES] =		//中心と角の三角形インデックス
	{
		{SWING_COURSE::LEFT_HIGH , SWING_COURSE::RIGHT_HIGH, SWING_COURSE::CENTER},		//上
		{SWING_COURSE::RIGHT_LOW , SWING_COURSE::LEFT_LOW  , SWING_COURSE::CENTER},		//下
		{SWING_COURSE::LEFT_LOW  , SWING_COURSE::LEFT_HIGH , SWING_COURSE::CENTER},		//左
		{SWING_COURSE::RIGHT_HIGH, SWING_COURSE::RIGHT_LOW , SWING_COURSE::CENTER},		//右
	};

	//ストライクゾーンの中心と四隅の三角形の中にスイング位置がある三角形を探し、
	//中心から二つの頂点に向かうベクトルを使用してスイング位置に行くために必要な割合を求め、
	//合成に使用するデータとして格納する
	for (int polygon_i = 0; polygon_i < POLYGON_COUNT; polygon_i++)
	{
		//ストライクゾーンの中心と角を結ぶ三角形の中にSWING_POSITIONが入っているかを判定
		const bool IS_IN_POLYGON = Math::IntersectLineToPolygon(SWING_POSITION, XMVectorSet(0, 0, -1.f, 0), 
			CORNER_POSITION[(int)INDEX[polygon_i][0]], CORNER_POSITION[(int)INDEX[polygon_i][1]], CORNER_POSITION[(int)INDEX[polygon_i][2]], nullptr);

		//入ってなければこの先の処理は行わなずにループの先頭へ
		if (!IS_IN_POLYGON)	continue;

		float percentages[POLYGON_COUNT - 1];		//ストライクゾーンの中心からポリゴンの角に向かうベクトルの比

		//ストライクゾーンの中心からポリゴンの角へのベクトルを使用してスイング位置に行くために必要な割合を求める
		Math::IntersectLineToLine2D(CORNER_POSITION[(int)SWING_COURSE::CENTER], SWING_POSITION,
			CORNER_POSITION[(int)INDEX[polygon_i][0]], -CORNER_POSITION[(int)INDEX[polygon_i][1]], &percentages[0], &percentages[1], nullptr);

		//compositeMap_に格納する
		compositeMap_.emplace(INDEX[polygon_i][0], percentages[0]);
		compositeMap_.emplace(INDEX[polygon_i][1], percentages[1]);

		break;
	}

	isSwing_ = true;
}

//アニメーションを最初の状態に戻す
void Batter::ResetAnimation()
{
	nextAnimationFrame_ = 0;																					//次フレームを0に
	isSwing_			= false;																				//スイングしていない
	hModelDraw_			= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(SWING_COURSE::CENTER));	//描画するモデルを中心を振るアニメーションに
	AllColliderActivate();																						//当たり判定を有効にする
	compositeMap_.clear();																						//アニメーション合成に使用するデータをクリア
	Model::DeformAnimationPose(hModelDraw_, nextAnimationFrame_);												//描画するモデルをアニメーションの最初の姿勢にする
}

//ストライクゾーンの設定
void Batter::SetStrikeZone() const
{
	/*
		ストライクゾーンの高さは構えた時の姿勢を基準に決定する

		上限：腰と高いほうの肩の高さの中間
		下限：低いほうの膝の高さ
	*/

	//ストライクゾーンの高さ設定に必要な体の部位の高さを取得
	const float LEFT_SHOULDER_HEIGHT	  = Model::GetBonePosition(hModelDraw_, "leftShoulder").vecY;		//左肩の高さ
	const float RIGHT_SHOULDER_HEIGHT	  = Model::GetBonePosition(hModelDraw_, "rightShoulder").vecY;		//右肩の高さ
	const float SHOULDER_HEIGHT			  = GetLargerValue(LEFT_SHOULDER_HEIGHT, RIGHT_SHOULDER_HEIGHT);	//高いほうを肩の高さとして扱う
	
	const float LEFT_KNEE_HEIGHT		  = Model::GetBonePosition(hModelDraw_, "leftKnee").vecY;			//左膝の高さ
	const float RIGHT_KNEE_HEIGHT		  = Model::GetBonePosition(hModelDraw_, "rightKnee").vecY;			//右膝の高さ
	const float KNEE_HEIGHT				  = GetSmallerValue(LEFT_KNEE_HEIGHT, RIGHT_KNEE_HEIGHT);			//低いほうを膝の高さとして扱う

	const float WAIST_HEIGHT			  = Model::GetBonePosition(hModelDraw_, "waist").vecY;				//腰の高さ

	//ストライクゾーンの高さの上限と下限を取得
	const float UPPER_LIMIT				  = GetMiddleValue(SHOULDER_HEIGHT, WAIST_HEIGHT);					//ストライクゾーンの高さの上限
	const float LOWER_LIMIT				  = KNEE_HEIGHT;													//ストライクゾーンの高さの下限

	//ストライクゾーンの中心の高さと全体の高さを取得
	const float STRIKE_ZONE_CENTER_HEIGHT = GetMiddleValue(UPPER_LIMIT, LOWER_LIMIT);						//ストライクゾーンの中心の高さ
	const float STRIKE_ZONE_HEIGHT		  = UPPER_LIMIT - LOWER_LIMIT;										//ストライクゾーンの高さ範囲

	//ストライクゾーンの高さを設定
	ExternalDataManager::SetStrikeZoneHeight(STRIKE_ZONE_HEIGHT, STRIKE_ZONE_CENTER_HEIGHT);
}

//当たり判定の位置の設定
void Batter::SetBatColliderPosition()
{
	//バットの両端の位置を設定
	XMVECTOR bothEndPosition[2] = { Model::GetBonePosition(hModelDraw_,"batGrip"),Model::GetBonePosition(hModelDraw_,"batTip") };
	SetColliderPosition(hColliderBat_, bothEndPosition);
}

//次フレームのアニメーションの姿勢を設定する
void Batter::SetNextAnimationPose()
{
	//変更前のバットの位置を取得
	prevBatPosition_ = Model::GetBonePosition(hModelDraw_, "batCenter");

	//中心を振るアニメーションを次フレームの姿勢にする
	const int MODEL_HANDLE_CENTER = Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(SWING_COURSE::CENTER));						//中心をスイングするバッターのモデルのハンドル番号
	Model::SetAnimationPose(MODEL_HANDLE_CENTER, nextAnimationFrame_);

	//中心を振るアニメーションと角を振るアニメーションを合成してアニメーションを自然にする
	//中心を振るアニメーションと角を振るアニメーションの次フレームの姿勢の差分を角を振るアニメーションの新しい姿勢として設定
	for (auto it : compositeMap_)
	{
		const SWING_COURSE	COMPOSITE_COURCE				= it.first;																			//合成するスイングコース
		const float			COMPOSITE_PERCENTAGE			= it.second;																		//中心を振るアニメーションからどれだけ角を振るアニメーションに寄せるか
		const int			MODEL_HANDLE_COMPOSITE_COURCE	= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(COMPOSITE_COURCE));	//合成するコースをスイングするバッターのモデルのハンドル番号

		//合成するアニメーションを次フレームの姿勢にする
		Model::SetAnimationPose(MODEL_HANDLE_COMPOSITE_COURCE, nextAnimationFrame_);

		//二つの姿勢の差分を新しい姿勢として設定
		Model::SetDifferencePose(MODEL_HANDLE_COMPOSITE_COURCE, MODEL_HANDLE_CENTER, COMPOSITE_PERCENTAGE);
	}

	//設定した姿勢の差分を中心を振るアニメーションの次フレームの姿勢と合成して指定した位置にスイングするアニメーションの次フレームの姿勢を作る
	for (auto it : compositeMap_)
	{
		const SWING_COURSE	COMPOSITE_COURCE				= it.first;																			//合成するスイングコース
		const int			MODEL_HANDLE_COMPOSITE_COURCE	= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(COMPOSITE_COURCE));	//合成するコースをスイングするバッターのモデルのハンドル番号

		//二つの姿勢の差分を新しい姿勢として設定
		Model::AddDifferencePose(MODEL_HANDLE_CENTER, MODEL_HANDLE_COMPOSITE_COURCE);
	}

	//合成した姿勢にモデルを変形させる
	Model::Deform(MODEL_HANDLE_CENTER);

	//描画するモデルを合成したモデルに設定する
	hModelDraw_ = MODEL_HANDLE_CENTER;
}

//二つの値の中間値(平均値)を取得
float Batter::GetMiddleValue(float value1, float value2) const
{
	const float HALF = 0.5f;				//半分
	return (value1 + value2) * HALF;
}

//二つの値のより大きいほうの値を取得
float Batter::GetLargerValue(float value1, float value2) const
{
	return (value1 > value2) ? value1 : value2;
}

//二つの値のより小さいほうの値を取得
float Batter::GetSmallerValue(float value1, float value2) const
{
	return (value1 < value2) ? value1 : value2;
}
