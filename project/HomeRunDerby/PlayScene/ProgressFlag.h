#pragma once

//ゲーム進行に関するフラグを管理するクラス
class ProgressFlag
{
private:

	char flag_;					//ゲーム進行に関するフラグ

public:

	ProgressFlag();
	~ProgressFlag();

	//各フラグの変更
	void Play();					//開始
	void HitBall();					//ボールがバットに当たった
	void GroundBall();				//ボールが地面に当たった
	void FoulBall();				//フィールドの範囲外にボールが出た
	void HomeRun();					//ホームラン
	void EndGame();					//ゲーム終了
	void ResetOnePlayFlag();		//１プレーごとに使用するフラグをリセット
	void ResetAllFlag();			//全てのフラグをリセット

	//各フラグの確認
	bool IsInPlay() const;			//プレー中か
	bool IsHitBall() const;			//ボールとバットが当たったか
	bool IsGroundBall() const;		//打球が地面に衝突したか
	bool IsFoulBall() const;		//フィールドの範囲外にボールが出たか
	bool IsHomeRun() const;			//ホームランか
	bool IsEndGame() const;			//ゲームを終了するか
};