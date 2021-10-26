#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../ExternalDataManager.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "Ball.h"

using DirectX::XMVECTOR;

//コンストラクタ
Pitcher::Pitcher(IGameObject* parent)
	: IGameObject(parent, "Pitcher"), nextAnimationFrame_(0), ballType_(-1), aimPosition_(DirectX::g_XMZero)
{
}

//初期化
void Pitcher::Initialize()
{
	//モデルデータのロード
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathPitcherModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);
	
	//ピッチャープレートの位置がピッチャーの位置
	transform_.position_ = ExternalDataManager::GetPitcherPlatePosition();

	//アニメーションを最初の状態にする
	ResetAnimation();
}

//更新
void Pitcher::Update()
{
	//現在がプレー中でなければ何もしない
	PlayScene* pPlayScene = (PlayScene*)pParent_;
	if (!pPlayScene->IsInPlay())	return;

	//次フレームの姿勢に変形する
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());				//ピッチャーのモデルのハンドル番号
	Model::DeformAnimationPose(MODEL_HANDLE, nextAnimationFrame_);

	//ピッチャーがボールを放していない場合の処理
	const int RELEASE_FRAME = ExternalDataManager::GetReleaseFrame();										//ピッチャーがボールを放すフレーム
	if (nextAnimationFrame_ <= RELEASE_FRAME)
	{
		Ball* pBall = (Ball*)pParent_->FindChildObject("Ball");									//ボールオブジェクト

		//手の位置をボールの位置に
		XMVECTOR drawPosistion = Model::GetBonePosition(MODEL_HANDLE, "PitchHand");				//投げる手の位置
		transform_.Calclation();																//行列の再計算
		drawPosistion = XMVector3TransformCoord(drawPosistion, transform_.GetWorldMatrix());	//ローカル座標からワールド座標に変換
		pBall->SetPosition(drawPosistion);

		//次のアニメーションフレームがボールを放すフレームならボールの更新を有効にする
		if (nextAnimationFrame_ == RELEASE_FRAME)	pBall->Activate();
	}

	nextAnimationFrame_++;
}

//描画
void Pitcher::Draw()
{
	//描画するモデルの状態を設定して描画
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//ピッチャーのモデルのハンドル番号
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE);
}

//解放
void Pitcher::Release()
{
}

//選択されている球種の取得
int Pitcher::GetBallType() const
{
	return ballType_;
}

//狙う位置の取得
XMVECTOR Pitcher::GetAimPosition() const
{
	return aimPosition_;
}

//ボールを放す位置の取得
XMVECTOR Pitcher::GetReleasePosition()
{
	//リリース時の姿勢を設定
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//ピッチャーのモデルのハンドル番号
	Model::SetAnimationPose(MODEL_HANDLE, ExternalDataManager::GetReleaseFrame());

	//投げる手の位置を取得
	XMVECTOR releasePosition = Model::GetBonePosition(MODEL_HANDLE, "PitchHand");				//投げる瞬間の姿勢時の投げる手の位置

	//ローカル座標からワールド座標に変換
	transform_.Calclation();																	//行列の再計算
	releasePosition = XMVector3TransformCoord(releasePosition, transform_.GetWorldMatrix());	//変換

	//姿勢を元に戻す
	Model::SetAnimationPose(MODEL_HANDLE, nextAnimationFrame_);

	return releasePosition;
}

//ボールを投げる準備
void Pitcher::InitializePitch()
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::operator*=;
	using DirectX::operator-=;

	//球種の選択
	ballType_ = SelectBallType();

	//投げる位置の選択
	aimPosition_ = SelectPitchCourse();

	const int		FAST_BALL	= 0;
	const float		ROTATE		= MinutesToFrame(ExternalDataManager::GetRotate(ballType_));							//rpfに変換した回転数
	const float		SPEED		= KilometreParHourToMetreParFrame(ExternalDataManager::GetBallSpeed(ballType_));		//mpfに変換した速度

	//ボールを放した時のボールの状態を設定する
	XMVECTOR pitchedMovement	=  ExternalDataManager::GetStrikeZoneCenter() + aimPosition_ - GetReleasePosition();	//投球時のボールの移動ベクトル
	pitchedMovement				=  DirectX::XMVector3Normalize(pitchedMovement);										//単位ベクトルに変換
	pitchedMovement				*= SPEED;																				//ベクトルの大きさを設定されている球速にする

	//選択されている球種がストレート以外であれば変化量を考慮してベクトルを変化させる
	if (ballType_ != FAST_BALL)
	{
		const XMVECTOR	PITCHER_TO_HOME_DIRECTION	= DirectX::XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());	//ピッチャーからホームへの単位ベクトル
		const XMVECTOR	CURVE_DIRECTION				= DirectX::XMVector3Cross(ExternalDataManager::GetRotateAxis(ballType_), PITCHER_TO_HOME_DIRECTION);							//変化方向の単位ベクトル
		const XMVECTOR	RELEASE_TO_HOME_BASE				= ExternalDataManager::GetStrikeZoneCenter() - GetReleasePosition();													//リリースポイントからストライクゾーンの中心へのベクトル
		const float		CURVE_AMOUNT				= ROTATE * CURVE_AMOUNT_ADJUSTMENT * RELEASE_TO_HOME_BASE.vecZ;																	//変化量

		//ボールの変化と逆方向に変化量だけずらす
		pitchedMovement -= CURVE_DIRECTION * CURVE_AMOUNT;
	}

	//ボールオブジェクトを探す
	Ball* pBall = (Ball*)(pParent_->FindChildObject("Ball"));					

	//ボール状態を放した瞬間の状態に設定する
	pBall->SetPitchState(pitchedMovement, ExternalDataManager::GetRotateAxis(ballType_), ROTATE);

	//アニメーションのフレームを0に
	nextAnimationFrame_ = 0;
}

//アニメーションを最初の状態に戻す
void Pitcher::ResetAnimation()
{
	//アニメーションのフレーム数を0にする
	nextAnimationFrame_		= 0;																//アニメーションのフレーム数を0に

	//最初の姿勢にする
	const int MODEL_HANDLE	= Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//ストライクゾーンのモデルハンドル
	Model::DeformAnimationPose(MODEL_HANDLE, nextAnimationFrame_);
}

//球種の選択
int Pitcher::SelectBallType() const
{
	//0~1の乱数取得
	const float	RANDOM_NUMBER	= GetRandomPercentage();						//0~1の乱数

	//取得した乱数から使用する球種を決める
	const int	BALL_TYPE_MAX	= ExternalDataManager::GetBallTypeCount();		//使用率の合計上限100%
	float		useRateCount	= 0;											//確認済み使用率の合計
	for (int ballType = 0; ballType < BALL_TYPE_MAX; ballType++)
	{
		//球種の使用率を取得して確認済みの使用率合計に加算する
		useRateCount += ExternalDataManager::GetUseRateBallType(ballType);

		//合計した確認済み使用率が乱数値を超えたら超えた時の球種を使用する
		if (useRateCount >= RANDOM_NUMBER)	return ballType;
	}

	return -1;
}

//どこに投げるかを決める
XMVECTOR Pitcher::SelectPitchCourse() const
{
	using DirectX::operator*;

	const float HALF			= 0.5f;													//半分
	const float HALF_WIDTH		= ExternalDataManager::GetStrikeZoneWidth() * HALF;		//ストライクゾーンの幅の半分
	const float HALF_HEIGHT		= ExternalDataManager::GetStrikeZoneHeight() * HALF;	//ストライクゾーンの高さの半分

	//ストライクゾーンの中心からどれだけ離れた位置に投げるかを決める
	XMVECTOR pitchCourse	= DirectX::g_XMZero;																	//ストライクゾーンの中心からの距離
	pitchCourse.vecX		= GetRandomPercentage() * ExternalDataManager::GetStrikeZoneWidth() - HALF_WIDTH;		//X方向
	pitchCourse.vecY		= GetRandomPercentage() * ExternalDataManager::GetStrikeZoneHeight() - HALF_HEIGHT;		//Y方向

	//現在のゲームモードに合わせて中心からの距離を調整して値を返す
	return pitchCourse * ExternalDataManager::GetCoursePercentage();
}

//0~1の乱数を取得
float Pitcher::GetRandomPercentage() const
{
	const int RANDOM_NUMBER_WIDTH = 100;					//乱数の幅

	return (float)(rand() % RANDOM_NUMBER_WIDTH) / RANDOM_NUMBER_WIDTH;
}

//km/hをm/fに単位変換
float Pitcher::KilometreParHourToMetreParFrame(float numeric) const
{
	const float TO_METRE = 1000.f;							//km -> m の変換
	const float TO_FRAME = powf(60.f, 3);					//時 -> フレーム（1/60秒）の変換

	const float TO_METRE_PAR_FRAME = TO_METRE / TO_FRAME;	//km/h -> m/frame の変換

	return numeric * TO_METRE_PAR_FRAME;
}

//*/minを*/fに単位変換
float Pitcher::MinutesToFrame(float numeric) const
{
	const float TO_FRAME = 1.f / 3600.f;					//min -> f の変換

	return numeric * TO_FRAME;
}
