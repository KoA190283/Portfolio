#pragma once
#include "ExternalDataManager.h"

//�Q�[���̃v���C�f�[�^��ێ�
namespace InternalPlayDataReservoir
{
	using ExternalDataManager::GAME_MODE;

	//������
	void InitializePlayData();

	//�Q�[���̃��[�h�ݒ�
	//�����Fmode			�Q�[���̃��[�h
	void SetGameMode(GAME_MODE mode);

	//���_�̒ǉ�
	//�����Fpoint			���_
	void AddScore(int point);

	//�c��񐔂�1���炷
	void SubtractRemainingCount();

	//�擾�֐�
	int			GetRemainingCount();	//�c��v���C��
	GAME_MODE	GetCurrentGameMode();	//�Q�[���̃��[�h
	int			GetScore();				//���_
};

