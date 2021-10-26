#include "InternalPlayDataReservoir.h"

//ゲームのプレイデータを保持
namespace InternalPlayDataReservoir
{
	namespace
	{
		GAME_MODE	currentGameMode_	= GAME_MODE::EASY;		//ゲームのモード
		int			score_				= 0;					//スコア
		int			remainingCount_		= 0;					//残り回数
	}

	//初期化
	void InitializePlayData()
	{
		score_			= 0;											
		remainingCount_ = ExternalDataManager::GetMaxRemainingCount();
	}

	//ゲームのモード設定
	void SetGameMode(GAME_MODE mode)
	{
		currentGameMode_ = mode;
	}

	//得点の追加
	void AddScore(int point)
	{
		score_ += point;
	}

	//残り回数を1減らす
	void SubtractRemainingCount()
	{
		remainingCount_--;
	}

	//残りプレイ回数
	int GetRemainingCount()
	{
		return remainingCount_;
	}

	//ゲームのモード
	GAME_MODE GetCurrentGameMode()
	{
		return currentGameMode_;
	}

	//得点
	int GetScore()
	{
		return score_;
	}
}