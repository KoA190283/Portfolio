#include "InternalPlayDataReservoir.h"

//�Q�[���̃v���C�f�[�^��ێ�
namespace InternalPlayDataReservoir
{
	namespace
	{
		GAME_MODE	currentGameMode_	= GAME_MODE::EASY;		//�Q�[���̃��[�h
		int			score_				= 0;					//�X�R�A
		int			remainingCount_		= 0;					//�c���
	}

	//������
	void InitializePlayData()
	{
		score_			= 0;											
		remainingCount_ = ExternalDataManager::GetMaxRemainingCount();
	}

	//�Q�[���̃��[�h�ݒ�
	void SetGameMode(GAME_MODE mode)
	{
		currentGameMode_ = mode;
	}

	//���_�̒ǉ�
	void AddScore(int point)
	{
		score_ += point;
	}

	//�c��񐔂�1���炷
	void SubtractRemainingCount()
	{
		remainingCount_--;
	}

	//�c��v���C��
	int GetRemainingCount()
	{
		return remainingCount_;
	}

	//�Q�[���̃��[�h
	GAME_MODE GetCurrentGameMode()
	{
		return currentGameMode_;
	}

	//���_
	int GetScore()
	{
		return score_;
	}
}