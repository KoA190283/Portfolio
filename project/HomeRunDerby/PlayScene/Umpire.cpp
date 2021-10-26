#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"
#include "Umpire.h"

using DirectX::XMVECTOR;

Umpire::Umpire()
{
}

Umpire::~Umpire()
{
}

//プレー開始
void Umpire::BeginPlay()
{
	flag_.Play();
}

//ワンプレーの終了
void Umpire::EndPlay()
{
	//プレー中でなければ何もしない
	if (!flag_.IsInPlay())	return;

	//ホームランであれば得点を追加する
	const int ADD_SCORE = 1;												//追加する得点数
	if (flag_.IsHomeRun())	InternalPlayDataReservoir::AddScore(ADD_SCORE);	

	//１プレーごとに使用するフラグをリセット
	flag_.ResetOnePlayFlag();								

	//残り回数を１減らす
	InternalPlayDataReservoir::SubtractRemainingCount();						
}

//ゲームの終了
void Umpire::EndGame()
{
	flag_.EndGame();
}

//プレー中かを取得する
bool Umpire::IsInPlay() const
{
	return flag_.IsInPlay();
}

//ボールの状態が打球かを取得する
bool Umpire::IsHitBall() const
{
	return flag_.IsHitBall();
}

//ボールの状態がゴロかを取得する
bool Umpire::IsGroundBall() const
{
	return flag_.IsGroundBall();
}

//ファールボールかを取得する
bool Umpire::IsFoulBall() const
{
	return flag_.IsFoulBall();
}

//ホームランかを取得する
bool Umpire::IsHomeRun() const
{
	return flag_.IsHomeRun();
}

//ゲームを終了するかを取得する
bool Umpire::IsEndGame() const
{
	return flag_.IsEndGame();
}

//ボールが何かと衝突したときにゲーム判定を行う
void Umpire::CollisionBall(const XMVECTOR & collisionPosision, const std::string& objectName)
{
	//ボールが衝突したオブジェクト名に応じた処理
	if (objectName == "Field")			CollisionBallToField(collisionPosision);	//フィールドに当たった
	if (objectName == "Batter")			flag_.HitBall();							//バッター（バット）に当たった
}

//ボールと地面が当たった時の処理
void Umpire::CollisionBallToField(const XMVECTOR & collisionPosision)
{
	using DirectX::operator-;

	if (flag_.IsGroundBall())	return;				//ボールが地面に落ちていれば何もしない

	//接地済みの状態に変更
	flag_.GroundBall();								

	//ホームからの中心線と衝突地点へのベクトルの角度を求める
	const XMVECTOR	CENTER_LINE		= DirectX::XMVector3Normalize(ExternalDataManager::GetPitcherPlatePosition() - ExternalDataManager::GetHomePlatePosition());	//ホームからの中心線
	const XMVECTOR	BALL_DIRECTION	= DirectX::XMVector3Normalize(collisionPosision);																				//ボールの方向
	const float		DOT_BALL_CENTER	= DirectX::XMVector3Dot(CENTER_LINE, BALL_DIRECTION).vecX;																		//ホームベースからセンター方向の線とボールの方向の内角
	const float		FAIR_ANGLE_MAX	= 0.5f;																															//フェア判定される最大角度

	//フィールドの範囲外に出ていたらスコアに対する影響はない
	const bool IS_FOUL_BALL = DOT_BALL_CENTER < FAIR_ANGLE_MAX;							//フィールドの範囲外に出ているか
	if (IS_FOUL_BALL)
	{
		flag_.FoulBall();
		return;	
	}

	//ホームランになる条件を取得する
	const float	DISTANCE_BALL		= DirectX::XMVector3Length(collisionPosision).vecX;	//ボールまでの距離
	const float HOME_RUN_DISTANCE	= ExternalDataManager::GetHomeRunDistance();		//ホームランになる距離
	
	//ホームランなら得点フラグをtrueにする
	const bool IS_HOME_RUN = DISTANCE_BALL > HOME_RUN_DISTANCE;							//ホームランか
	if (IS_HOME_RUN)	flag_.HomeRun();
}