#include "ExternalDataManager.h"

#include <vector>
#include <map>

#include "Engine/Global.h"
#include "InternalPlayDataReservoir.h"

//�O���t�@�C���̃f�[�^���Ǘ�
namespace ExternalDataManager
{
	//�����v�Z�Ŏg�p����I�u�W�F�N�g���Ƃ̌W���l
	struct Coefficient
	{
		float restitution;		//�����W��
		float friction;			//���C�W��

		Coefficient(): restitution(0), friction(0){}
	};

	//�X�g���C�N�]�[���̃f�[�^
	struct StrikeZoneData
	{
		std::string	filePath;	//���f���f�[�^�̃t�@�C���p�X
		XMVECTOR	center;		//���S�_
		float		width;		//��
		float		height;		//����

		StrikeZoneData() : center(DirectX::g_XMZero), width(0), height(0){}
	};

	//�{�[���̃f�[�^
	struct BallData
	{
		std::string	filePath;	//���f���f�[�^�̃t�@�C���p�X
		float		radius;		//�{�[���̔��a(m)
		float		weight;		//�{�[���̏d��(kg)

		BallData() : radius(0), weight(0) {}
	};

	//�o�b�^�[�̃f�[�^
	struct BatterData
	{
		std::string	filePath[(int)SWING_COURSE::MAX];	//���f���f�[�^�̃t�@�C���p�X
		float		batRadius;							//�o�b�g�̔��a(m)
		float		power;								//�p���[�l
		int			impactFrame;						//�ł��͂������t���[����

		BatterData() : batRadius(0), power(0), impactFrame(0) {}
	};

	//���킲�Ƃ̃f�[�^
	struct BallTypeData
	{
		float		speed;				//����
		float		rotate;				//��]��
		XMVECTOR	rotateAxis;			//��]��

		BallTypeData() : speed(0), rotate(0), rotateAxis(DirectX::g_XMZero) {}
	};

	//�s�b�`���[�̃f�[�^
	struct PitcherData
	{
		std::string						filePath;				//���f���f�[�^�̃t�@�C���p�X
		int								releaseFrame;			//�{�[��������^�C�~���O�̃t���[����
		std::vector<float>				throwProbabilityList;	//���킲�Ƃ̓����銄��
		std::vector<BallTypeData*>		ballTypeDataList;		//���킲�Ƃ̃f�[�^�̃��X�g

		PitcherData() : releaseFrame(0) {}
	};

	//�Q�[�����[�h�̃f�[�^
	struct GameModeData
	{
		int			remainingCount;			//�c���
		float		gameSpeedAcceleration;	//�Q�[�����̓����̑��x�{��
		float		coursePercentage;		//�s�b�`���[���ǂ��܂ŃR�[�X��_���邩

		GameModeData() : remainingCount(0), gameSpeedAcceleration(0), coursePercentage(0) {}
	};

	//�t�B�[���h�̕\���Ɏg�p���郂�f���̎��
	enum class FIELD_MODEL
	{
		FIELD_OBUJECT = 0,	//�t�B�[���h�ɒu���I�u�W�F�N�g�i���C���A�x�[�X�Ȃǁj
		GROUND,				//�n��
		MAX,
	};

	//�t�B�[���h�̃f�[�^
	struct FieldData
	{
		std::string	filePath[(int)FIELD_MODEL::MAX];	//���f���f�[�^�̃t�@�C���p�X
		XMVECTOR	homePlatePosition;					//�z�[���v���[�g�̈ʒu
		XMVECTOR	pitcherPlatePosition;				//�s�b�`���[�v���[�g�̈ʒu
		float		homeRunDistance;					//�z�[�������ɂȂ鋗��
		
		FieldData() : homePlatePosition(DirectX::g_XMZero), pitcherPlatePosition(DirectX::g_XMZero), homeRunDistance(0) {}
	};

	//�����f�[�^�ւ̃t�@�C���p�X
	enum class NUMBER_FILE_PATH
	{
		FILE_PATH = 0,		//�t�@�C���܂ł̃p�X
		EXTENSION,			//�g���q
		MAX,
	};

	//�w�i�f�[�^
	struct BackGroundData
	{
		std::string	filePath;	//���f���f�[�^�̃t�@�C���p�X
		int			shaderType;	//�g�p����V�F�[�_
		BackGroundData() : shaderType(0){}
	};

	////////////////////
	//�ϐ�
	std::vector<std::string>			 sceneImageFilePathList_;						//�V�[�����Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X
	std::vector<BackGroundData>			 backGroundDataList_;							//�V�[�����Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X
	std::map<std::string, Coefficient*>	 objectCoefficientMap_;							//�I�u�W�F�N�g�����L�[�ɂ��ăI�u�W�F�N�g���Ƃ̌W���l���i�[
	StrikeZoneData*						 pStrikeZoneData_		= nullptr;				//�X�g���C�N�]�[���̃f�[�^
	BallData*							 pBallData_				= nullptr;				//�{�[���̃f�[�^
	BatterData*							 pBatterData_			= nullptr;				//�o�b�^�[�̃f�[�^
	PitcherData*						 pPitcherData_			= nullptr;				//�s�b�`���[�̃f�[�^
	GameModeData*						 pGameModeData_			= nullptr;				//�Q�[�����[�h�Ɋւ���f�[�^
	FieldData*							 pFieldData_			= nullptr;				//�t�B�[���h�Ɋւ���f�[�^
	std::string							 numberFilePath_[(int)NUMBER_FILE_PATH::MAX];	//�����̉摜�ւ̃t�@�C���p�X

	//�^�C�g���V�[���Ŏg�p����f�[�^�̏�����
	void InitializeTitleSceneData()
	{
		//csv�t�@�C������^�C�g���V�[���Ŏg�p����f�[�^��ǂݍ���
		LoadTitleSceneImageFilePath("Data/TitleSceneImageFilePath.csv");	//�V�[�����Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
	}

	//�v���C�V�[���Ŏg�p����f�[�^�̏�����
	void InitializePlaySceneData()
	{
		//csv�t�@�C������v���C�V�[���Ŏg�p����f�[�^��ǂݍ���
		LoadPlaySceneImageFilePath("Data/PlaySceneImageFilePath.csv");		//�V�[�����Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
		LoadCoefficient("Ball", "Data/Coefficient/BallCoefficient.csv");	//�{�[���̌W���l�̓ǂݍ���
		LoadCoefficient("Field", "Data/Coefficient/FieldCoefficient.csv");	//�n�ʂ̌W���l�̓ǂݍ���
		LoadCoefficient("Batter", "Data/Coefficient/BatCoefficient.csv");	//�o�b�g�̌W���l�̓ǂݍ���
		LoadStrikeZoneData("Data/StrikeZoneData.csv");						//�X�g���C�N�]�[���̃f�[�^�̓ǂݍ���
		LoadBallData("Data/BallData.csv");									//�{�[���̃f�[�^�̓ǂݍ���
		LoadBatterData("Data/BatterData.csv");								//�o�b�^�[�̃f�[�^�̓ǂݍ���
		LoadPitcherData("Data/PitcherData.csv");							//�s�b�`���[�̃f�[�^�̓ǂݍ���
		LoadGameModeData("Data/GameModeData.csv");							//�Q�[�����[�h�̃f�[�^�̓ǂݍ���
		LoadFieldData("Data/FieldData.csv");								//�t�B�[���h�̃f�[�^�̓ǂݍ���
	}

	//���U���g�V�[���Ŏg�p����f�[�^�̏�����
	void InitializeResultSceneData()
	{
		//csv�t�@�C�����烊�U���g�V�[���Ŏg�p����f�[�^��ǂݍ���
		LoadResultSceneImageFilePath("Data/ResultSceneImageFilePath.csv");	//�V�[�����Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
	}

	//�X�g���C�N�]�[���̍����̐ݒ�
	void SetStrikeZoneHeight(float height, const float center)
	{
		pStrikeZoneData_->height		= height;
		pStrikeZoneData_->center.vecY	= center;
	}

	//�t�B�[���h�f�[�^�̐ݒ�
	void SetFieldData(const XMVECTOR& homePlatePosition, const XMVECTOR& pitcherPlatePosition, float homeRunDistance)
	{
		pFieldData_->homePlatePosition		= homePlatePosition;
		pFieldData_->pitcherPlatePosition	= pitcherPlatePosition;
		pFieldData_->homeRunDistance		= homeRunDistance;
	}

	//�^�C�g���V�[���Ŏg�p����C���[�W�f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathTitleSceneImage(TITLE_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//�v���C�V�[���Ŏg�p����C���[�W�f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathPlaySceneImage(PLAY_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//���U���g�V�[���Ŏg�p����C���[�W�f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathResultSceneImage(RESULT_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//�����̉摜�f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathNumberImage(int number)
	{
		return numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH] + std::to_string(number) + numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION];
	}

	//�����W���̎擾
	float GetCoefficientRestitution(const std::string& objectName)
	{
		return objectCoefficientMap_[objectName]->restitution;
	}
	
	//���C�W���̎擾
	float GetCoefficientFriction(const std::string& objectName)
	{
		return objectCoefficientMap_[objectName]->friction;
	}

	//����̎g�p�����擾
	float GetUseRateBallType(int type)
	{
		return pPitcherData_->throwProbabilityList[type];
	}

	//����̋������擾
	float GetBallSpeed(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->speed;
	}

	//����̉�]�����擾
	float GetRotate(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->rotate;
	}

	//����̉�]�����擾
	XMVECTOR GetRotateAxis(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->rotateAxis;
	}

	//�o�b�^�[�̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathBatterModel(SWING_COURSE course)
	{
		return pBatterData_->filePath[(int)course];
	}

	//�w�i�̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathBackGroundModel(int index)
	{
		return backGroundDataList_[index].filePath;
	}

	//�g�p����V�F�[�_���擾
	int GetShaderTypeBackGround(int index)
	{
		return backGroundDataList_[index].shaderType;
	}

	//�s�b�`���[�̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathPitcherModel()
	{
		return pPitcherData_->filePath;
	}

	//�{�[���̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathBallModel()
	{
		return pBallData_->filePath;
	}

	//�X�g���C�N�]�[���̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathStrikeZoneModel()
	{
		return pStrikeZoneData_->filePath;
	}

	//�t�B�[���h�ɒu���I�u�W�F�N�g�̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathFieldObjectModel()
	{
		return pFieldData_->filePath[(int)FIELD_MODEL::FIELD_OBUJECT];
	}

	//�n�ʂ̃��f���f�[�^�ւ̃t�@�C���p�X���擾
	std::string GetFilePathGroundModel()
	{
		return pFieldData_->filePath[(int)FIELD_MODEL::GROUND];
	}

	//�X�g���C�N�]�[���̕����擾
	float GetStrikeZoneWidth()
	{
		return pStrikeZoneData_->width;
	}

	//�X�g���C�N�]�[���̍������擾
	float GetStrikeZoneHeight()
	{
		return pStrikeZoneData_->height;
	}

	//�X�g���C�N�]�[���̒��S�ʒu���擾
	XMVECTOR GetStrikeZoneCenter()
	{
		return pStrikeZoneData_->center;
	}

	//�{�[���̔��a���擾
	float GetBallRadius()
	{
		return pBallData_->radius;
	}

	//�{�[���̏d�����擾
	float GetBallWeight()
	{
		return pBallData_->weight;
	}

	//�o�b�g�̔��a���擾
	float GetBatRadius()
	{
		return pBatterData_->batRadius;
	}

	//�o�b�^�[�̃p���[���擾
	float GetBatterPower()
	{
		return pBatterData_->power;
	}

	//�o�b�^�[�̗̂͂��ł������t���[�������擾
	int GetBatterImpactFrame()
	{
		return pBatterData_->impactFrame;
	}

	//�s�b�`���[����������̐����擾
	int GetBallTypeCount()
	{
		return (int)pPitcherData_->ballTypeDataList.size();
	}

	//�s�b�`���[���{�[��������t���[�������擾
	int GetReleaseFrame()
	{
		return pPitcherData_->releaseFrame;
	}

	//�c��񐔂��擾
	int GetRemainingCount()
	{
		return pGameModeData_->remainingCount;
	}

	//�Q�[�����̓����̑��x���擾
	float GetGameSpeedAcceleration()
	{
		return pGameModeData_->gameSpeedAcceleration;
	}

	//�s�b�`���[���ǂ��܂ŃR�[�X��_�������擾
	float GetCoursePercentage()
	{
		return pGameModeData_->coursePercentage;
	}

	//�ő�v���C�񐔂��擾
	int GetMaxRemainingCount()
	{
		return pGameModeData_->remainingCount;
	}

	//�z�[���v���[�g�̈ʒu���擾
	XMVECTOR GetHomePlatePosition()
	{
		return pFieldData_->homePlatePosition;
	}

	//�s�b�`���[�v���[�g�̈ʒu���擾
	XMVECTOR GetPitcherPlatePosition()
	{
		return pFieldData_->pitcherPlatePosition;
	}

	//�z�[�������ɂȂ鋗�����擾
	float GetHomeRunDistance()
	{
		return pFieldData_->homeRunDistance;
	}

	//�^�C�g���V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
	void LoadTitleSceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																//�f�[�^�������

		//�^�C�g���V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv titleSceneImageFilePathData;														//�^�C�g���V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���f�[�^
		assert(titleSceneImageFilePathData.Load(fileName));										//CSV�t�@�C���̓ǂݍ���

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���N���A����
		sceneImageFilePathList_.clear();

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���i�[
		for (int row = 0; row < (int)TITLE_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = titleSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//�摜�ւ̃t�@�C���p�X
			sceneImageFilePathList_.push_back(filePath);										//���X�g�Ɋi�[
		}
	}

	//�v���C�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
	void LoadPlaySceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																//�f�[�^�������

		//�v���C�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv playSceneImageFilePathData;															//�v���C�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���f�[�^
		assert(playSceneImageFilePathData.Load(fileName));										//CSV�t�@�C���̓ǂݍ���

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���N���A����
		sceneImageFilePathList_.clear();

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���i�[
		for (int row = 0; row < (int)PLAY_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = playSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//�摜�ւ̃t�@�C���p�X
			sceneImageFilePathList_.push_back(filePath);										//���X�g�Ɋi�[
		}

		const int ROW_NUMBER_FILE_PATH						= (int)PLAY_SCENE_IMAGE::MAX;																						//�����̉摜�ւ̃t�@�C���p�X���i�[����Ă���s
		numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH]	= playSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::FILE_PATH);	//�����̉摜������t�@�C���܂ł̃t�@�C���p�X
		numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION]	= playSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::EXTENSION);	//�����̉摜�̊g���q
	}

	//���U���g�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�̓ǂݍ���
	void LoadResultSceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																	//�f�[�^�������

		//���U���g�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv resultSceneImageFilePathData;															//���U���g�V�[���Ŏg�p����摜�f�[�^�ւ̃t�@�C���p�X�������Ă���CSV�t�@�C���f�[�^
		assert(resultSceneImageFilePathData.Load(fileName));										//CSV�t�@�C���̓ǂݍ���

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���N���A����
		sceneImageFilePathList_.clear();

		//�V�[���Ŏg�p����摜�ւ̃t�@�C���p�X�f�[�^���i�[
		for (int row = 0; row < (int)RESULT_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = resultSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//�摜�ւ̃t�@�C���p�X
			sceneImageFilePathList_.push_back(filePath);											//���X�g�Ɋi�[
		}

		const int ROW_NUMBER_FILE_PATH						= (int)RESULT_SCENE_IMAGE::MAX;																						//�����̉摜�ւ̃t�@�C���p�X���i�[����Ă���s
		numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH]	= resultSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::FILE_PATH);	//�����̉摜������t�@�C���܂ł̃t�@�C���p�X
		numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION]	= resultSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::EXTENSION);	//�����̉摜�̊g���q
	}

	//�W���l�f�[�^�̓ǂݍ���
	void LoadCoefficient(const std::string& objectName, std::string const& fileName)
	{
		bool IS_EXISTS = objectCoefficientMap_.find(objectName) != objectCoefficientMap_.end();			//objectName�̃f�[�^�����݂��邩
		if (IS_EXISTS) return;																			//���[�h�ς݂Ȃ牽�����Ȃ�

		enum class ROW 
		{
			RESTITUTION = 0,																			//�����W��������s
			FRICTION,																					//���C�W��������s
			MAX,
		};
		const int COLUMN_DATA = 1;																		//�f�[�^�������

		//�W���l�f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv coefficientData;																			//�W���l�������Ă���CSV�t�@�C���f�[�^
		assert(coefficientData.Load(fileName));															//CSV�t�@�C���̓ǂݍ���

		//�V�����W���l�f�[�^�̍쐬
		Coefficient* pCoefficient	= new Coefficient;													//�V�����W���l
		pCoefficient->restitution	= coefficientData.GetFloatDeta((int)ROW::RESTITUTION, COLUMN_DATA);	//�����W���l���i�[
		pCoefficient->friction		= coefficientData.GetFloatDeta((int)ROW::FRICTION, COLUMN_DATA);	//���C�W���l���i�[

		objectCoefficientMap_.emplace(objectName, pCoefficient);										//�W���l�N���X�����i�[
	}

	//�X�g���C�N�]�[���̃f�[�^�̓ǂݍ���
	void LoadStrikeZoneData(const std::string& fileName)
	{
		//�s���Ƃɓ����Ă���f�[�^
		enum class ROW 
		{
			FILE_PATH = 0,														//���f���f�[�^�ւ̃t�@�C���p�X������s
			POSITION,															//�ʒu������s
			WIDTH,																//�����̒l������s
			MAX,
		};

		const int COLUMN_DATA = 1;												//�f�[�^�������

		//�X�g���C�N�]�[���̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv strikeZoneData;														//�X�g���C�N�]�[���̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(strikeZoneData.Load(fileName));									//CSV�t�@�C���̓ǂݍ���
			
		//�X�g���C�N�]�[���̗p�̃��������Ȃ���ΐV�����m��
		if (pStrikeZoneData_ == nullptr) pStrikeZoneData_ = new StrikeZoneData;				

		//�X�g���C�N�]�[���̃f�[�^���i�[
		pStrikeZoneData_->filePath	= strikeZoneData.GetStringDeta((int)ROW::FILE_PATH, COLUMN_DATA);		//���f���f�[�^�̃t�@�C���p�X
		pStrikeZoneData_->center	= GetVector3CsvData(strikeZoneData, (int)ROW::POSITION, COLUMN_DATA);	//���S���W
		pStrikeZoneData_->width		= strikeZoneData.GetFloatDeta((int)ROW::WIDTH, COLUMN_DATA);			//��
	}

	//�{�[���̃f�[�^�̓ǂݍ���
	void LoadBallData(const std::string& fileName)
	{
		//�s���Ƃɓ����Ă���f�[�^
		enum class ROW 
		{
			FILE_PATH = 0,																	//���f���f�[�^�ւ̃t�@�C���p�X������s
			RADIUS,																			//���a�̒l������s
			WEIGHT,																			//�d���̒l������s
			MAX,
		};

		const int COLUMN_DATA = 1;															//�f�[�^�������

		//�{�[���̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv ballData;																		//�{�[���̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(ballData.Load(fileName));													//CSV�t�@�C���̓ǂݍ���

		//�{�[���f�[�^�p�̃��������Ȃ���ΐV�����m��
		if (pBallData_ == nullptr) pBallData_ = new BallData;				

		//�{�[���̃f�[�^���i�[
		pBallData_->filePath	= ballData.GetStringDeta((int)ROW::FILE_PATH, COLUMN_DATA);	//���f���f�[�^�̃t�@�C���p�X
		pBallData_->radius		= ballData.GetFloatDeta((int)ROW::RADIUS, COLUMN_DATA);		//���a
		pBallData_->weight		= ballData.GetFloatDeta((int)ROW::WEIGHT, COLUMN_DATA);		//�d��
	}

	//�o�b�^�[�̃f�[�^�̓ǂݍ���
	void LoadBatterData(const std::string& fileName)
	{
		//�s���Ƃɓ����Ă���f�[�^
		enum class ROW 
		{
			FILE_PATH = 0,															//���f���f�[�^�ւ̃t�@�C���p�X������s
			RADIUS = (int)SWING_COURSE::MAX,										//���a�̒l������s
			POWER,																	//�p���[�̒l������s
			IMPACT_FRAME,															//�ł��͂������t���[����
			MAX,
		};

		const int COLUMN_DATA = 1;													//�f�[�^�������

		//�o�b�^�[�̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv batterData;																//�o�b�^�[�̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(batterData.Load(fileName));											//CSV�t�@�C���̓ǂݍ���

		//�o�b�^�[�f�[�^�p�̃��������Ȃ���ΐV�����m��
		if (pBatterData_ == nullptr) pBatterData_ = new BatterData;					

		//�o�b�^�[�̃f�[�^���i�[
		//�A�j���[�V�����̃X�C���O����R�[�X���Ƃ̃��f���f�[�^�̃t�@�C���p�X���i�[
		for (int i = 0; i < (int)SWING_COURSE::MAX; i++)
			pBatterData_->filePath[i]	= batterData.GetStringDeta((int)ROW::FILE_PATH + i, COLUMN_DATA);

		pBatterData_->batRadius			= batterData.GetFloatDeta((int)ROW::RADIUS, COLUMN_DATA);			//�o�b�g�̔��a
		pBatterData_->power				= batterData.GetFloatDeta((int)ROW::POWER, COLUMN_DATA);			//�p���[�l
		pBatterData_->impactFrame		= batterData.GetIntegerDeta((int)ROW::IMPACT_FRAME, COLUMN_DATA);	//�ł��͂������t���[����
	}

	//�s�b�`���[�̃f�[�^�̓ǂݍ���
	void LoadPitcherData(const std::string& fileName)
	{
		//�s���Ƃɓ����Ă���f�[�^
		enum class ROW 
		{
			FILE_PATH			= 0,										//���f���f�[�^�ւ̃t�@�C���p�X������s
			RELEASE_FRAME,													//�{�[��������t���[����
			BALL_TYPE			= 4,										//����
			MAX,
		};

		//�񂲂Ƃɓ����Ă���f�[�^
		enum class COLUMN
		{
			PITCHER_DATA		= 1,										//���t���[���ڂŃ{�[���������
			BALL_TYPE_NAME		= 0,										//���햼
			THROW_PROBABILITY,												//�����銄��
			MAX,
		};

		//�s�b�`���[�̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv pitcherData;													//�s�b�`���[�̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(pitcherData.Load(fileName));									//CSV�t�@�C���̓ǂݍ���

		//�s�b�`���[�f�[�^�p�̃��������Ȃ���ΐV�����m��
		if (pPitcherData_ == nullptr) pPitcherData_ = new PitcherData;

		//�f�[�^�̊i�[
		pPitcherData_->filePath		= pitcherData.GetStringDeta((int)ROW::FILE_PATH, (int)COLUMN::PITCHER_DATA);		//���f���f�[�^�̃t�@�C���p�X
		pPitcherData_->releaseFrame = pitcherData.GetIntegerDeta((int)ROW::RELEASE_FRAME, (int)COLUMN::PITCHER_DATA);	//�{�[��������t���[�������i�[

		//�s�b�`���[���g�p���鋅��f�[�^�̓ǂݍ���
		const int ROW_END = (int)pitcherData.GetHeight();					//�Ō�̍s
		for (int row = (int)ROW::BALL_TYPE; row < (int)ROW_END; row++)
		{
			const std::string	BALL_TYPE_NAME		= pitcherData.GetStringDeta(row, (int)COLUMN::BALL_TYPE_NAME);		//���햼
			const float			THROW_PROBABILITY	= pitcherData.GetFloatDeta(row, (int)COLUMN::THROW_PROBABILITY);	//BALL_TYPE_NAME���g�p���銄��

			//���킲�Ƃ̃f�[�^�����[�h
			LoadBallTypeData("Data/BallType/" + BALL_TYPE_NAME + ".csv");

			//���[�h����������g�p���銄�����i�[
			pPitcherData_->throwProbabilityList.push_back(THROW_PROBABILITY);
		}
	}

	//�ω����̃f�[�^�̓ǂݍ���
	void LoadBallTypeData(const std::string& fileName)
	{
		//�񂲂Ƃɓ����Ă���f�[�^�̓��e
		enum class ROW
		{
			SPEED = 0,											//����
			ROTATE,												//��]��
			ROTATE_AXIS,										//��]��
			MAX,
		};

		const int COLUMN_DATA = 1;								//�f�[�^�������

		//���킲�Ƃ̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv ballTypeData;										//����̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(ballTypeData.Load(fileName));					//CSV�t�@�C���̓ǂݍ���

		//�V��������f�[�^���쐬
		BallTypeData* newBallTypeData = new BallTypeData;

		//�e�f�[�^���擾
		newBallTypeData->speed				= ballTypeData.GetFloatDeta((int)ROW::SPEED, COLUMN_DATA);					//�������i�[
		newBallTypeData->rotate				= ballTypeData.GetFloatDeta((int)ROW::ROTATE, COLUMN_DATA);					//��]�����i�[
		newBallTypeData->rotateAxis			= GetVector3CsvData(ballTypeData, (int)ROW::ROTATE_AXIS, COLUMN_DATA);		//��]���̃x�N�g���i�[

		//�����x�N�g���𐳋K�����ĒP�ʃx�N�g���ɕϊ�
		newBallTypeData->rotateAxis			= DirectX::XMVector3Normalize(newBallTypeData->rotateAxis);

		//�s�b�`���[�̎�������Ƃ��Ċi�[
		pPitcherData_->ballTypeDataList.push_back(newBallTypeData);
	}

	//�Q�[�����[�h�ŕϓ�����Q�[�����e�Ɋւ���f�[�^�̓ǂݍ���
	void LoadGameModeData(const std::string& fileName)
	{
		const int	ROW_DATA_BEGIN	= 1;
		const int	ROW_GAME_MODE	= (int)InternalPlayDataReservoir::GetCurrentGameMode() + ROW_DATA_BEGIN;	//�f�[�^������s
		
		//�񂲂Ƃɓ����Ă���f�[�^�̓��e
		enum class COLUMN
		{
			GAME_SPEED_ACCELERATION = 1,										//�Q�[���X�s�[�h
			REMAINING_COUNT,													//�c��񐔐�
			COURSE_PERCENTAGE,													//�s�b�`���[���ǂꂾ���R�[�X�����邩
			MAX,
		};

		//�Q�[�����[�h�Ɋւ���f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv gameModeData;														//�Q�[�����[�h�̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(gameModeData.Load(fileName));									//CSV�t�@�C���̓ǂݍ���

		//�Q�[�����[�h�Ɋւ���f�[�^�p�̃��������Ȃ���ΐV�����m��
		if (pGameModeData_ == nullptr) pGameModeData_ = new GameModeData;

		//�Q�[�����[�h�ɍ��킹���f�[�^�̎擾
		pGameModeData_->gameSpeedAcceleration	= gameModeData.GetFloatDeta(ROW_GAME_MODE, (int)COLUMN::GAME_SPEED_ACCELERATION);	//�Q�[���X�s�[�h
		pGameModeData_->remainingCount			= gameModeData.GetIntegerDeta(ROW_GAME_MODE, (int)COLUMN::REMAINING_COUNT);			//�c��񐔐�
		pGameModeData_->coursePercentage		= gameModeData.GetFloatDeta(ROW_GAME_MODE, (int)COLUMN::COURSE_PERCENTAGE);			//�s�b�`���[���ǂ��܂ŃR�[�X��_���邩
	}

	//�t�B�[���h�̃f�[�^�̓ǂݍ���
	void LoadFieldData(const std::string& fileName)
	{
		//�s���Ƃɓ����Ă���f�[�^
		enum class ROW
		{
			FILE_PATH_FIELD_OBJECT = 0,											//�t�B�[���h�ɒu����郂�f���f�[�^�ւ̃t�@�C���p�X������s
			FILE_PATH_GROUND,													//�n�ʂ̃��f���f�[�^�ւ̃t�@�C���p�X������s
			PITCHER_PLATE_POSITION,												//�s�b�`���[�v���[�g�̈ʒu������s
			HOME_PLATE_POSITION,												//�z�[���v���[�g�̈ʒu������s
			HOMERUN_DISTANCE,													//�z�[�������ɂȂ鋗��������s
			MAX,
		};

		const int COLUMN_DATA = 1;												//�f�[�^�������

		//�o�b�^�[�̃f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv fieldData;															//�o�b�^�[�̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(fieldData.Load(fileName));										//CSV�t�@�C���̓ǂݍ���

		//�o�b�^�[�f�[�^�p�̃��������Ȃ���ΐV�����m��
		if (pFieldData_ == nullptr) pFieldData_ = new FieldData;

		//�t�B�[���h�̃f�[�^���i�[
		pFieldData_->filePath[(int)FIELD_MODEL::FIELD_OBUJECT]	= fieldData.GetStringDeta((int)ROW::FILE_PATH_FIELD_OBJECT, COLUMN_DATA);		//�t�B�[���h�ɒu����郂�f���f�[�^�̃t�@�C���p�X
		pFieldData_->filePath[(int)FIELD_MODEL::GROUND]			= fieldData.GetStringDeta((int)ROW::FILE_PATH_GROUND, COLUMN_DATA);				//�n�ʂ̃��f���f�[�^�̃t�@�C���p�X
		pFieldData_->pitcherPlatePosition						= GetVector3CsvData(fieldData, (int)ROW::PITCHER_PLATE_POSITION, COLUMN_DATA);	//�s�b�`���[�v���[�g�̈ʒu
		pFieldData_->homePlatePosition							= GetVector3CsvData(fieldData, (int)ROW::HOME_PLATE_POSITION, COLUMN_DATA);		//�z�[���v���[�g�̈ʒu
		pFieldData_->homeRunDistance							= fieldData.GetFloatDeta((int)ROW::HOMERUN_DISTANCE, COLUMN_DATA);				//�z�[���v���[�g�̈ʒu
	}

	//�w�i�`��p�̃f�[�^��ǂݍ���
	void LoadBackGroundData(const std::string& fileName)
	{
		const int	ROW_DATA_FIRST = 1;											//�f�[�^������ŏ��̍s

		//�񂲂Ƃɓ����Ă���f�[�^�̓��e
		enum class COLUMN
		{
			FILE_PATH = 0,														//���f���f�[�^�ւ̃t�@�C���p�X������s
			SHADER_TYPE,														//�g�p����V�F�[�_
			MAX,
		};

		//�w�i�Ɋւ���f�[�^�������Ă���CSV�t�@�C���̓ǂݍ���
		Csv backGroundData;														//�w�i�̃f�[�^�������Ă���CSV�t�@�C���f�[�^
		assert(backGroundData.Load(fileName));									//CSV�t�@�C���̓ǂݍ���

		const int ROW_DATA_END = (int)backGroundData.GetHeight();				//�f�[�^������Ō�̍s

		//�Q�[�����[�h�Ɋւ���f�[�^�p�̃��������Ȃ���ΐV�����m��
		backGroundDataList_.clear();

		//�w�i�f�[�^����s�������X�g�ɒǉ�
		int count = 0;
		for (int row = ROW_DATA_FIRST; row < ROW_DATA_END; row++)
		{
			BackGroundData newData;				//�V�K�f�[�^

			//�w�i�f�[�^�̎擾
			newData.filePath	= backGroundData.GetStringDeta(row, (int)COLUMN::FILE_PATH);		//�t�@�C���p�X
			newData.shaderType	= backGroundData.GetIntegerDeta(row, (int)COLUMN::SHADER_TYPE);		//�V�F�[�_
			
			//���X�g�ɒǉ�
			backGroundDataList_.push_back(newData);
		}
	}

	//csv�t�@�C���ɓ����s��3�A���Ŋi�[����Ă���l���x�N�g����x,y,z�Ƃ��Ď擾����
	XMVECTOR GetVector3CsvData(Csv& csv, int row, int column)
	{
		//�x�N�g���v�f
		enum class VECTOR
		{
			X = 0,	//X
			Y,		//Y
			Z,		//Z
			MAX,
		};

		//�e�v�f�̒l���擾����
		XMVECTOR out	= DirectX::g_XMZero;									//�o�͂���l
		out.vecX		= csv.GetFloatDeta(row, column + (int)VECTOR::X);
		out.vecY		= csv.GetFloatDeta(row, column + (int)VECTOR::Y);
		out.vecZ		= csv.GetFloatDeta(row, column + (int)VECTOR::Z);

		return out;
	}

	//���
	void Release()
	{
		SAFE_DELETE(pStrikeZoneData_);
		SAFE_DELETE(pBatterData_);
		SAFE_DELETE(pBallData_);
		if (pPitcherData_ != nullptr)
		{
			for (auto it : pPitcherData_->ballTypeDataList)
				SAFE_DELETE(it);
			pPitcherData_->ballTypeDataList.clear();

			SAFE_DELETE(pPitcherData_);
		}

		for (auto it : objectCoefficientMap_)
			SAFE_DELETE(it.second);
		objectCoefficientMap_.clear();
	}
}