#pragma once
#include"../Engine/IGameObject.h"
#include"../Engine/Transform.h"
#include "ProgressFlag.h"

//審判クラス
class Umpire
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	ProgressFlag		flag_;				//試合に関するフラグ

public:

	Umpire();
	~Umpire();

	//ゲームの状態に関する関数
	void BeginPlay();				//プレー開始
	void EndPlay();					//プレーの終了
	void EndGame();					//ゲームの終了

	bool IsInPlay() const;			//プレー中か
	bool IsHitBall() const;			//ボールの状態が打球か
	bool IsGroundBall() const;		//ボールの状態がゴロか
	bool IsFoulBall() const;		//ボールの状態がファールボールか
	bool IsHomeRun() const;			//ホームランか
	bool IsEndGame() const;			//ゲームを終了するか

	//ボールが何かと衝突したときにゲーム判定を行う
	//引数１：collisionPosision	衝突位置
	//引数２：objectName		衝突したオブジェクトの名前
	void CollisionBall(const XMVECTOR& collisionPosision, const std::string& objectName);

private:

	//ボールと地面が当たった時の処理
	//引数：collisionPosision	衝突位置
	void CollisionBallToField(const XMVECTOR& collisionPosision);		//ボールと地面が当たった
};