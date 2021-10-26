#pragma once
#include <string>
#include "Engine/Transform.h"
#include "Engine/Csv.h"

//�O���t�@�C���̃f�[�^���Ǘ�
namespace ExternalDataManager
{
	using DirectX::XMVECTOR;

	//��Փx���[�h
	enum class GAME_MODE
	{
		EASY = 0,		//�Ղ���
		NORMAL,			//����
		HARD,			//���
		MAX,
	};

	//�X�C���O�̃R�[�X����
	enum class SWING_COURSE
	{
		LEFT_HIGH = 0,	//����
		LEFT_LOW,		//����
		RIGHT_HIGH,		//�E��
		RIGHT_LOW,		//�E��
		CENTER,			//���S
		MAX,
	};

	//�^�C�g���V�[���Ŏg�p����摜�̎��
	enum class TITLE_SCENE_IMAGE
	{
		TITLE = 0,		//�^�C�g��
		MODE_EASY,		//��Փx�I���i�Ղ����j
		MODE_NORMAL,	//��Փx�I���i���ʁj
		MODE_HARD,		//��Փx�I���i����j
		MAX,
	};

	//�v���C�V�[���Ŏg�p����摜�̎��
	enum class PLAY_SCENE_IMAGE
	{
		SCORE_BOARD = 0,//����
		PURPOSE,		//�ړI�\���Ɏg�p���镶����摜
		REMAINING,		//�c��̋����\���Ɏg�p���镶����摜
		SCORE,			//�X�R�A�\���Ɏg�p���镶����摜
		HOME_RUN,		//�z�[�������ł��邱�Ƃ�\���̂Ɏg�p���镶����摜
		MAX,
	};

	//���U���g�V�[���Ŏg�p����摜�̎��
	enum class RESULT_SCENE_IMAGE
	{
		SCORE_BOARD = 0,//����
		SCORE,			//�X�R�A�\���Ɏg�p���镶����摜
		END_GAME,		//�Q�[�����I������
		TITLE_BACK,		//�^�C�g���ɖ߂�
		MAX,
	};

	const int DECIMAL_BASE = 10;		//10�i���̒�

	////////////////////
	//�V�[�����ƂɎg�p����f�[�^�̏������֐�
	void InitializeTitleSceneData();	//�^�C�g���V�[��
	void InitializePlaySceneData();		//�v���C�V�[��
	void InitializeResultSceneData();	//���U���g�V�[��


	////////////////////
	//�ݒ�֐�

	//�X�g���C�N�]�[���̐ݒ�
	//�����P�Fheight		�X�g���C�N�]�[���̍���
	//�����Q�Fcenter		�X�g���C�N�]�[���̒��S�ʒu
	void SetStrikeZoneHeight(float height, float center);

	//�t�B�[���h�f�[�^�̐ݒ�
	//�����P�FhomePlatePosition		�z�[���v���[�g�̈ʒu
	//�����Q�FpitcherPlatePosition	�s�b�`���[�v���[�g�̈ʒu
	//�����R�FhomeRunDistance		�z�[�������ɂȂ鋗��
	void SetFieldData(const XMVECTOR& homePlatePosition, const XMVECTOR& pitcherPlatePosition, float homeRunDistance);

	////////////////////
	//�擾�֐�

	//�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X���擾
	//�����Fimage			�摜�̎��
	std::string GetFilePathTitleSceneImage(TITLE_SCENE_IMAGE image);		//�^�C�g���V�[��
	std::string GetFilePathPlaySceneImage(PLAY_SCENE_IMAGE image);			//�v���C�V�[��
	std::string GetFilePathResultSceneImage(RESULT_SCENE_IMAGE image);		//���U���g�V�[��

	//�����̉摜�f�[�^�ւ̃t�@�C���p�X���擾
	//�����Fnumber			�摜�̐���
	std::string GetFilePathNumberImage(int number);

	//�W���l�̎擾
	//�����FobjectName		�擾����l���Ȃ�̃I�u�W�F�N�g�̂��̂�
	float		GetCoefficientRestitution(const std::string& objectName);	//�����W��
	float		GetCoefficientFriction(const std::string& objectName);		//���C�W��
	
	//���킲�Ƃ̒l���擾
	//�����Ftype			���ڂ̋��킩
	float		GetUseRateBallType(int type);								//�g�p��					
	float		GetBallSpeed(int type);										//����
	float		GetRotate(int type);										//��]��
	XMVECTOR	GetRotateAxis(int type);									//��]��

	//�o�b�^�[�̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	//�����Fcourse			���f���̃A�j���[�V�������X�C���O����R�[�X
	std::string GetFilePathBatterModel(SWING_COURSE course);

	//�w�i�f�[�^�̎擾
	std::string GetFilePathBackGroundModel(int index);						//�w�i�̃��f���f�[�^�ւ̃t�@�C���p�X
	int			GetShaderTypeBackGround(int index);							//�g�p����V�F�[�_
	
	//�������̎擾�֐�
	std::string GetFilePathPitcherModel();									//�s�b�`���[�̃��f���f�[�^�ւ̃t�@�C���p�X
	std::string GetFilePathBallModel();										//�{�[���̃��f���f�[�^�ւ̃t�@�C���p�X
	std::string GetFilePathStrikeZoneModel();								//�X�g���C�N�]�[���̃��f���f�[�^�ւ̃t�@�C���p�X
	std::string GetFilePathFieldObjectModel();								//�t�B�[���h�ɒu���I�u�W�F�N�g�̃��f���f�[�^�ւ̃t�@�C���p�X
	std::string GetFilePathGroundModel();									//�n�ʂ̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	float		GetStrikeZoneWidth();										//�X�g���C�N�]�[���̕�
	float		GetStrikeZoneHeight();										//�X�g���C�N�]�[���̍���
	XMVECTOR	GetStrikeZoneCenter();										//�X�g���C�N�]�[���̒��S�ʒu
	float		GetBallRadius();											//�{�[���̔��a
	float		GetBallWeight();											//�{�[���̏d��
	float		GetBatRadius();												//�o�b�g�̔��a
	float		GetBatterPower();											//�o�b�^�[�̃p���[
	int			GetBatterImpactFrame();										//�o�b�^�[�̗̂͂��ł������t���[����
	int			GetBallTypeCount();											//�s�b�`���[����������̐�
	int			GetReleaseFrame();											//�s�b�`���[���{�[��������t���[����
	float		GetGameSpeedAcceleration();									//�Q�[�����̓����̑��x
	float		GetCoursePercentage();										//�s�b�`���[���ǂ��܂ŃR�[�X��_����
	int			GetMaxRemainingCount();										//�ő�v���C��
	XMVECTOR	GetHomePlatePosition();										//�z�[���v���[�g�̈ʒu
	XMVECTOR	GetPitcherPlatePosition();									//�s�b�`���[�v���[�g�̈ʒu
	float		GetHomeRunDistance();										//�z�[�������ɂȂ鋗��

	

	////////////////////
	//�ǂݍ��݊֐�

	//�I�u�W�F�N�g�����L�[�ɂ��ăI�u�W�F�N�g���Ƃ̌W���l���i�[
	//�����P�FobjectName	�ǂݍ��ރf�[�^�̃I�u�W�F�N�g��
	//�����Q�FfileName		�ǂݍ��ރt�@�C�����i�p�X�j
	void LoadCoefficient(const std::string& objectName, std::string const & fileName);

	//�t�@�C������l��ǂݍ���
	//�����FfileName		�ǂݍ��ރt�@�C�����i�p�X�j
	void LoadTitleSceneImageFilePath(const std::string& fileName);			//�^�C�g���V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X
	void LoadPlaySceneImageFilePath(const std::string& fileName);			//�v���C�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X
	void LoadResultSceneImageFilePath(const std::string& fileName);			//���U���g�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X
	void LoadStrikeZoneData(const std::string& fileName);					//�X�g���C�N�]�[���̃f�[�^
	void LoadBallData(const std::string& fileName);						//�{�[���̃f�[�^
	void LoadBatterData(const std::string& fileName);						//�o�b�^�[�̃f�[�^
	void LoadPitcherData(const std::string& fileName);						//�s�b�`���[�̃f�[�^
	void LoadBallTypeData(const std::string& fileName);					//�ω����̃f�[�^
	void LoadGameModeData(const std::string& fileName);					//�Q�[�����[�h�ŕϓ�����Q�[�����e�Ɋւ���f�[�^
	void LoadFieldData(const std::string& fileName);						//�t�B�[���h�̃f�[�^
	void LoadBackGroundData(const std::string& fileName);					//�w�i�`��p�̃f�[�^


	//csv�t�@�C���ɓ����s��3�A���Ŋi�[����Ă���l���x�N�g����x,y,z�Ƃ��Ď擾����
	//�����P�Fcsv	�ǂݍ��ރf�[�^�̃I�u�W�F�N�g��
	//�����Q�Frow		�擾����f�[�^�̂���s
	//�����R�Fcolumn	�擾����f�[�^�̂����
	//�ߒl	�Fcsv����ǂݎ�����x�N�g���l
	XMVECTOR GetVector3CsvData(Csv& csv, int row, int column);

	//���
	void Release();
};