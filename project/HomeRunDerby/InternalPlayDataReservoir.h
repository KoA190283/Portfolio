#pragma once
#include "ExternalDataManager.h"

//ゲームのプレイデータを保持
namespace InternalPlayDataReservoir
{
	using ExternalDataManager::GAME_MODE;

	//初期化
	void InitializePlayData();

	//ゲームのモード設定
	//引数：mode			ゲームのモード
	void SetGameMode(GAME_MODE mode);

	//得点の追加
	//引数：point			得点
	void AddScore(int point);

	//残り回数を1減らす
	void SubtractRemainingCount();

	//取得関数
	int			GetRemainingCount();	//残りプレイ回数
	GAME_MODE	GetCurrentGameMode();	//ゲームのモード
	int			GetScore();				//得点
};

