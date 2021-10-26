#include <stdlib.h>
#include <Windows.h>

#include "../ExternalDataManager.h"
#include "Direct3D.h"
#include "Global.h"
#include "Image.h"
#include "Input.h"
#include "Model.h"
#include "RootJob.h"

#pragma comment(lib, "winmm.lib")

//�萔�錾
const char* WIN_CLASS_NAME	= "HomeRun Derby";		//�E�B���h�E�N���X��

//�v���g�^�C�v�錾
HWND InitWindow(HINSTANCE hInstance, int nCmdShow);											//�E�B���h�E�̏���
bool InitEngine(HWND hWnd);																	//�G���W�������̏�����
void MessLoop(HWND hWnd);																	//���b�Z�[�W���[�v
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);				//�E�B���h�E�v���V�[�W��

//�N���X�|�C���^
static RootJob* pRootJob = nullptr;															//�Q�[���I�u�W�F�N�g�̃��[�g

//�G���g���[�|�C���g
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	srand((unsigned)time(NULL));					//�����ݒ�
	SetCurrentDirectory("Assets");					//�J�����g�f�B���N�g���̕ύX

	//�E�B���h�E�֘A�̏���
	HWND hWnd = InitWindow(hInstance, nCmdShow);	//�E�B���h�E�̏���
	ShowWindow(hWnd, nCmdShow);						//�E�B���h�E��\��

	//�G���W�������̏�����
	if (!InitEngine(hWnd)) return 0;

	//���b�Z�[�W���[�v
	MessLoop(hWnd);

	//���I�m�ۂ����ϐ��̉���E�폜
	pRootJob->ReleaseSub();
	SAFE_DELETE(pRootJob);
	Input::Release();
	Direct3D::Release();

	return 0;
}

//�E�B���h�E�̏���
HWND InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	//�E�B���h�E�N���X�i�݌v�}�j���쐬
	WNDCLASSEX wc;
	wc.cbSize		 = sizeof(WNDCLASSEX);					//���̍\���̂̃T�C�Y
	wc.hInstance	 = hInstance;							//�C���X�^���X�n���h��
	wc.lpszClassName = WIN_CLASS_NAME;						//�E�B���h�E�N���X��
	wc.lpfnWndProc	 = WndProc;								//�E�B���h�E�v���V�[�W��
	wc.style		 = CS_VREDRAW | CS_HREDRAW;				//�X�^�C���i�f�t�H���g�j
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);		//�A�C�R��
	wc.hIconSm		 = LoadIcon(NULL, IDI_WINLOGO);			//�������A�C�R��
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);			//�}�E�X�J�[�\��
	wc.lpszMenuName	 = NULL;								//���j���[�i�Ȃ��j
	wc.cbClsExtra	 = 0;									//�V�X�e���������ŊǗ�����WNDCLASS�\���̂ɒǉ����郁�����̃T�C�Y
	wc.cbWndExtra	 = 0;									//�E�C���h�E��\�������f�[�^�\���̂ɒǉ����郁����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //�w�i�i���j
	RegisterClassEx(&wc);									//�N���X��o�^

	//�E�B���h�E�T�C�Y�̌v�Z
	RECT winRect = { 0, 0, Global::WINDOW_WIDTH, Global::WINDOW_HEIGHT };	//�E�B���h�E�̃T�C�Y���
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);					//����
	int	 winW	 = winRect.right - winRect.left;							//�E�B���h�E��
	int  winH	 = winRect.bottom - winRect.top;							//�E�B���h�E����

	//�E�B���h�E���쐬
	HWND hWnd = CreateWindow(
		WIN_CLASS_NAME,		 //�E�B���h�E�N���X��
		WIN_CLASS_NAME,		 //�^�C�g���o�[�ɕ\��������e
		WS_OVERLAPPEDWINDOW, //�X�^�C���i���ʂ̃E�B���h�E�j
		CW_USEDEFAULT,       //�\���ʒu���i���܂����j
		CW_USEDEFAULT,       //�\���ʒu��i���܂����j
		winW,				 //�E�B���h�E����
		winH,				 //�E�B���h�E��
		NULL,                //�e�E�C���h�E�i�Ȃ��j
		NULL,                //���j���[�i�Ȃ��j
		hInstance,           //�C���X�^���X
		NULL                 //�p�����[�^�i�Ȃ��j
	);

	return hWnd;
}

//�G���W�������̏�����
bool InitEngine(HWND hWnd)
{
	//Direct3D������
	if (FAILED(Direct3D::Initialize(hWnd)))
	{
		Direct3D::Release();
		return false;
	}

	//Input������
	if (FAILED(Input::Initialize(hWnd)))
	{
		Input::Release();
		Direct3D::Release();
		return false;
	}

	//���[�g�W���u������
	pRootJob = new RootJob;
	pRootJob->Initialize();

	return true;
}

//���b�Z�[�W���[�v�i�����N����̂�҂j
void MessLoop(HWND hWnd)
{
	MSG msg;						//���b�Z�[�W
	ZeroMemory(&msg, sizeof(msg));	//������

	//�I�����b�Z�[�W��������I��
	while (msg.message != WM_QUIT)
	{
		//���b�Z�[�W����
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);							//���z�L�[�R�[�h�����ۂ̕����ɕϊ�
			DispatchMessage(&msg);							//�E�B���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o����
		}

		//���b�Z�[�W�Ȃ�
		else
		{
			timeBeginPeriod(1);								//�擾���鎞�Ԃ̐��x���グ��

			DWORD		 nowTime		 = timeGetTime();	//���݂̎���
			static DWORD startTime		 = nowTime;			//�J�n����
			static DWORD lastUpdateTime	 = nowTime;			//�ŏI�X�V����
			static DWORD countFps		 = 0;				//�X�V��

			//1000�~���b�i1�b�j������
			const int	MILLISECOND = 1000;					//1/1000�b

			//�K��X�V���x��葁��������Ȃɂ����Ȃ�
			DWORD updateTime = (nowTime - lastUpdateTime) * Global::GAME_FPS;	//�O��̍X�V����̎���
			if (updateTime <= MILLISECOND) continue;							//�K�葬�x��葁����΍X�V�����Ƀ��[�v�̐擪��

			lastUpdateTime = nowTime;						//�ŏI�X�V���Ԃ̍X�V
			countFps++;										//�X�V��
		
			//���̍X�V
			Input::Update();								//���͏��̍X�V
			pRootJob->UpdateSub();							//GameObject�̍X�V

			if (pRootJob->IsEndGame()) break;				//�I�����肪true�Ȃ烁�b�Z�[�W���[�v�𔲂���

			//�`�揈��
			Direct3D::BeginDraw();							//�`����̏�����
			pRootJob->DrawSub();							//GameObject�̕`��
			Direct3D::EndDraw();							//�`��

			Sleep(1);										//1 m/sec�x��
			timeEndPeriod(1);								//���Ԏ擾�̐��x��߂�
		}
	}

	//�A�Z�b�g�f�[�^�̉��
	ExternalDataManager::Release();
	Model::Release();
	Image::Release();
}

//�E�B���h�E�v���V�[�W���i�������������ɂ�΂��֐��j
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//���b�Z�[�W�ŕ���
	switch (msg)
	{
	//�}�E�X��������
	case WM_MOUSEMOVE:	
		Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));	//�}�E�X�̈ʒu��ݒ�
		return 0;

	//�E�B���h�E������ꂽ
	case WM_DESTROY:	
		PostQuitMessage(0);											//�v���O�����I��
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}