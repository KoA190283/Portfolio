#pragma once
#include <windows.h>

#define SAFE_DELETE(p)			{if ((p)!=nullptr) { delete (p); (p)=nullptr;}}		//�|�C���^�̍폜
#define SAFE_DELETE_ARRAY(p)	{if ((p)!=nullptr) { delete[] (p); (p)=nullptr;}}	//�|�C���^�z��̍폜
#define SAFE_RELEASE(p)			{if ((p)!=nullptr) { p->Release(); (p)=nullptr;}}	//�|�C���^��Release�֐��Ăяo����폜

//�����I�u�W�F�N�g�ɎQ�Ƃ����萔�E�֐��Q
namespace Global
{
	//ini�t�@�C������ǂݍ��񂾂���
	static const int WINDOW_WIDTH	= GetPrivateProfileInt("SCREEN", "Width", 800, ".\\setup.ini");		//�X�N���[���̕�
	static const int WINDOW_HEIGHT	= GetPrivateProfileInt("SCREEN", "Height", 600, ".\\setup.ini");	//�X�N���[���̍���
	static const int GAME_FPS		= GetPrivateProfileInt("GAME", "Fps", 60, ".\\setup.ini");			//FPS�i��ʍX�V���x�j
}