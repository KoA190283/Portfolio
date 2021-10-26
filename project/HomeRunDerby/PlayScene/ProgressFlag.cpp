#include "ProgressFlag.h"

/*
	ProgressFlag::flag_のデータ格納について

	//ビット数：管理するフラグ

	1:プレー中か
	2:ボールとバットが当たったか
	3:フライボールか
	4:フィールドの範囲外にボールが出たか
	
	5:ホームランか
	6:ゲームを終了するか
	7:空データ
	8:空データ
*/

const char IN_PLAY			= 0x01;	//プレー中
const char HIT_BALL			= 0x02;	//ボールとバットが当たった
const char GROUND_BALL		= 0x04;	//グラウンドに当たった
const char FOUL_BALL		= 0x08;	//フィールドの範囲外にボールが出た
const char HOME_RUN			= 0x10;	//ホームラン
const char END_GAME			= 0x20;	//ゲームを終了する
const char ONE_PLAY_ZONE	= 0x1F;	//ワンプレーごとに使用するフラグの範囲

ProgressFlag::ProgressFlag() :flag_(0)
{
}

ProgressFlag::~ProgressFlag()
{
}

//開始
void ProgressFlag::Play()
{
	flag_ |= IN_PLAY;
}

//ボールがバットに当たった
void ProgressFlag::HitBall()
{
	flag_ |= HIT_BALL;
}

//ボールが地面に当たった
void ProgressFlag::GroundBall()
{
	flag_ |= GROUND_BALL;
}

//フィールドの範囲外にボールが出た
void ProgressFlag::FoulBall()
{
	flag_ |= FOUL_BALL;
}

//ホームラン
void ProgressFlag::HomeRun()
{
	flag_ |= HOME_RUN;
}

//ゲーム終了
void ProgressFlag::EndGame()
{
	flag_ |= END_GAME;
}

//１プレーごとに使用するフラグをリセット
void ProgressFlag::ResetOnePlayFlag()
{
	flag_ &= !ONE_PLAY_ZONE;
}

//全てのフラグをリセット
void ProgressFlag::ResetAllFlag()
{
	flag_ = 0;
}

//プレー中か
bool ProgressFlag::IsInPlay() const
{
	return flag_ & IN_PLAY;
}

//ボールとバットが当たったか
bool ProgressFlag::IsHitBall() const
{
	return flag_ & HIT_BALL;
}

//打球が地面に衝突したか
bool ProgressFlag::IsGroundBall() const
{
	return flag_ & GROUND_BALL;
}

//フィールドの範囲外にボールが出たか
bool ProgressFlag::IsFoulBall() const
{
	return flag_ & FOUL_BALL;
}

//ホームランか
bool ProgressFlag::IsHomeRun() const
{
	return flag_ & HOME_RUN;
}

//ゲームを終了するか
bool ProgressFlag::IsEndGame() const
{
	return flag_ & END_GAME;
}
