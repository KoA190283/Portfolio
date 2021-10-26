#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"
#include "Umpire.h"

//プレイシーンの管理
class PlayScene : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	std::list<IUserInterface*>	userInterfaceList_;	//UIクラスのリスト
	Umpire*						pUmpire_;			//審判クラス
	int							timer_;				//経過時間を計測

public:
	//コンストラクタ
	PlayScene(IGameObject* parent);

	//初期化
	void Initialize() override;

	//ボールが何かと衝突したときの処理
	//引数１：collisionPosision	衝突時のボールの位置
	//引数２：objectName		衝突したオブジェクトの名前
	void CollisionBall(const XMVECTOR& collisionPosision, const std::string& objectName);

	//ゲーム内の速度を取得
	//戻値：ゲーム内の加速度（1が現実速度）
	float GetGameSpeedAcceleration() const;

	//プレーの状態を取得
	bool IsInPlay() const;		//プレー中か
	bool IsHitBall() const;		//打球か
	bool IsHomeRun() const;		//ホームランか

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;

	//子孫の描画を行った後に行う描画処理
	void DrawAfterChildren() override;

	//進行状況を更新
	void UpdateProgress();

	//プレー開始
	void BeginPlay();

	//プレー終了
	void EndPlay();

	//カメラを最初の位置に戻す
	void ResetCamera() const;
};