#include "ExternalDataManager.h"

#include <vector>
#include <map>

#include "Engine/Global.h"
#include "InternalPlayDataReservoir.h"

//外部ファイルのデータを管理
namespace ExternalDataManager
{
	//物理計算で使用するオブジェクトごとの係数値
	struct Coefficient
	{
		float restitution;		//反発係数
		float friction;			//摩擦係数

		Coefficient(): restitution(0), friction(0){}
	};

	//ストライクゾーンのデータ
	struct StrikeZoneData
	{
		std::string	filePath;	//モデルデータのファイルパス
		XMVECTOR	center;		//中心点
		float		width;		//幅
		float		height;		//高さ

		StrikeZoneData() : center(DirectX::g_XMZero), width(0), height(0){}
	};

	//ボールのデータ
	struct BallData
	{
		std::string	filePath;	//モデルデータのファイルパス
		float		radius;		//ボールの半径(m)
		float		weight;		//ボールの重さ(kg)

		BallData() : radius(0), weight(0) {}
	};

	//バッターのデータ
	struct BatterData
	{
		std::string	filePath[(int)SWING_COURSE::MAX];	//モデルデータのファイルパス
		float		batRadius;							//バットの半径(m)
		float		power;								//パワー値
		int			impactFrame;						//最も力が加わるフレーム数

		BatterData() : batRadius(0), power(0), impactFrame(0) {}
	};

	//球種ごとのデータ
	struct BallTypeData
	{
		float		speed;				//球速
		float		rotate;				//回転数
		XMVECTOR	rotateAxis;			//回転軸

		BallTypeData() : speed(0), rotate(0), rotateAxis(DirectX::g_XMZero) {}
	};

	//ピッチャーのデータ
	struct PitcherData
	{
		std::string						filePath;				//モデルデータのファイルパス
		int								releaseFrame;			//ボールを放すタイミングのフレーム数
		std::vector<float>				throwProbabilityList;	//球種ごとの投げる割合
		std::vector<BallTypeData*>		ballTypeDataList;		//球種ごとのデータのリスト

		PitcherData() : releaseFrame(0) {}
	};

	//ゲームモードのデータ
	struct GameModeData
	{
		int			remainingCount;			//残り回数
		float		gameSpeedAcceleration;	//ゲーム内の動きの速度倍率
		float		coursePercentage;		//ピッチャーがどこまでコースを狙えるか

		GameModeData() : remainingCount(0), gameSpeedAcceleration(0), coursePercentage(0) {}
	};

	//フィールドの表示に使用するモデルの種類
	enum class FIELD_MODEL
	{
		FIELD_OBUJECT = 0,	//フィールドに置くオブジェクト（ライン、ベースなど）
		GROUND,				//地面
		MAX,
	};

	//フィールドのデータ
	struct FieldData
	{
		std::string	filePath[(int)FIELD_MODEL::MAX];	//モデルデータのファイルパス
		XMVECTOR	homePlatePosition;					//ホームプレートの位置
		XMVECTOR	pitcherPlatePosition;				//ピッチャープレートの位置
		float		homeRunDistance;					//ホームランになる距離
		
		FieldData() : homePlatePosition(DirectX::g_XMZero), pitcherPlatePosition(DirectX::g_XMZero), homeRunDistance(0) {}
	};

	//数字データへのファイルパス
	enum class NUMBER_FILE_PATH
	{
		FILE_PATH = 0,		//ファイルまでのパス
		EXTENSION,			//拡張子
		MAX,
	};

	//背景データ
	struct BackGroundData
	{
		std::string	filePath;	//モデルデータのファイルパス
		int			shaderType;	//使用するシェーダ
		BackGroundData() : shaderType(0){}
	};

	////////////////////
	//変数
	std::vector<std::string>			 sceneImageFilePathList_;						//シーン内で使用する画像データへのファイルパス
	std::vector<BackGroundData>			 backGroundDataList_;							//シーン内で使用する画像データへのファイルパス
	std::map<std::string, Coefficient*>	 objectCoefficientMap_;							//オブジェクト名をキーにしてオブジェクトごとの係数値を格納
	StrikeZoneData*						 pStrikeZoneData_		= nullptr;				//ストライクゾーンのデータ
	BallData*							 pBallData_				= nullptr;				//ボールのデータ
	BatterData*							 pBatterData_			= nullptr;				//バッターのデータ
	PitcherData*						 pPitcherData_			= nullptr;				//ピッチャーのデータ
	GameModeData*						 pGameModeData_			= nullptr;				//ゲームモードに関するデータ
	FieldData*							 pFieldData_			= nullptr;				//フィールドに関するデータ
	std::string							 numberFilePath_[(int)NUMBER_FILE_PATH::MAX];	//数字の画像へのファイルパス

	//タイトルシーンで使用するデータの初期化
	void InitializeTitleSceneData()
	{
		//csvファイルからタイトルシーンで使用するデータを読み込む
		LoadTitleSceneImageFilePath("Data/TitleSceneImageFilePath.csv");	//シーン内で使用する画像データへのファイルパスの読み込み
	}

	//プレイシーンで使用するデータの初期化
	void InitializePlaySceneData()
	{
		//csvファイルからプレイシーンで使用するデータを読み込む
		LoadPlaySceneImageFilePath("Data/PlaySceneImageFilePath.csv");		//シーン内で使用する画像データへのファイルパスの読み込み
		LoadCoefficient("Ball", "Data/Coefficient/BallCoefficient.csv");	//ボールの係数値の読み込み
		LoadCoefficient("Field", "Data/Coefficient/FieldCoefficient.csv");	//地面の係数値の読み込み
		LoadCoefficient("Batter", "Data/Coefficient/BatCoefficient.csv");	//バットの係数値の読み込み
		LoadStrikeZoneData("Data/StrikeZoneData.csv");						//ストライクゾーンのデータの読み込み
		LoadBallData("Data/BallData.csv");									//ボールのデータの読み込み
		LoadBatterData("Data/BatterData.csv");								//バッターのデータの読み込み
		LoadPitcherData("Data/PitcherData.csv");							//ピッチャーのデータの読み込み
		LoadGameModeData("Data/GameModeData.csv");							//ゲームモードのデータの読み込み
		LoadFieldData("Data/FieldData.csv");								//フィールドのデータの読み込み
	}

	//リザルトシーンで使用するデータの初期化
	void InitializeResultSceneData()
	{
		//csvファイルからリザルトシーンで使用するデータを読み込む
		LoadResultSceneImageFilePath("Data/ResultSceneImageFilePath.csv");	//シーン内で使用する画像データへのファイルパスの読み込み
	}

	//ストライクゾーンの高さの設定
	void SetStrikeZoneHeight(float height, const float center)
	{
		pStrikeZoneData_->height		= height;
		pStrikeZoneData_->center.vecY	= center;
	}

	//フィールドデータの設定
	void SetFieldData(const XMVECTOR& homePlatePosition, const XMVECTOR& pitcherPlatePosition, float homeRunDistance)
	{
		pFieldData_->homePlatePosition		= homePlatePosition;
		pFieldData_->pitcherPlatePosition	= pitcherPlatePosition;
		pFieldData_->homeRunDistance		= homeRunDistance;
	}

	//タイトルシーンで使用するイメージデータへのファイルパスを取得
	std::string GetFilePathTitleSceneImage(TITLE_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//プレイシーンで使用するイメージデータへのファイルパスを取得
	std::string GetFilePathPlaySceneImage(PLAY_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//リザルトシーンで使用するイメージデータへのファイルパスを取得
	std::string GetFilePathResultSceneImage(RESULT_SCENE_IMAGE image)
	{
		return sceneImageFilePathList_[(int)image];
	}

	//数字の画像データへのファイルパスを取得
	std::string GetFilePathNumberImage(int number)
	{
		return numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH] + std::to_string(number) + numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION];
	}

	//反発係数の取得
	float GetCoefficientRestitution(const std::string& objectName)
	{
		return objectCoefficientMap_[objectName]->restitution;
	}
	
	//摩擦係数の取得
	float GetCoefficientFriction(const std::string& objectName)
	{
		return objectCoefficientMap_[objectName]->friction;
	}

	//球種の使用率を取得
	float GetUseRateBallType(int type)
	{
		return pPitcherData_->throwProbabilityList[type];
	}

	//球種の球速を取得
	float GetBallSpeed(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->speed;
	}

	//球種の回転数を取得
	float GetRotate(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->rotate;
	}

	//球種の回転軸を取得
	XMVECTOR GetRotateAxis(int type)
	{
		return pPitcherData_->ballTypeDataList[type]->rotateAxis;
	}

	//バッターのモデルデータへのファイルパスを取得
	std::string GetFilePathBatterModel(SWING_COURSE course)
	{
		return pBatterData_->filePath[(int)course];
	}

	//背景のモデルデータへのファイルパスを取得
	std::string GetFilePathBackGroundModel(int index)
	{
		return backGroundDataList_[index].filePath;
	}

	//使用するシェーダを取得
	int GetShaderTypeBackGround(int index)
	{
		return backGroundDataList_[index].shaderType;
	}

	//ピッチャーのモデルデータへのファイルパスを取得
	std::string GetFilePathPitcherModel()
	{
		return pPitcherData_->filePath;
	}

	//ボールのモデルデータへのファイルパスを取得
	std::string GetFilePathBallModel()
	{
		return pBallData_->filePath;
	}

	//ストライクゾーンのモデルデータへのファイルパスを取得
	std::string GetFilePathStrikeZoneModel()
	{
		return pStrikeZoneData_->filePath;
	}

	//フィールドに置くオブジェクトのモデルデータへのファイルパスを取得
	std::string GetFilePathFieldObjectModel()
	{
		return pFieldData_->filePath[(int)FIELD_MODEL::FIELD_OBUJECT];
	}

	//地面のモデルデータへのファイルパスを取得
	std::string GetFilePathGroundModel()
	{
		return pFieldData_->filePath[(int)FIELD_MODEL::GROUND];
	}

	//ストライクゾーンの幅を取得
	float GetStrikeZoneWidth()
	{
		return pStrikeZoneData_->width;
	}

	//ストライクゾーンの高さを取得
	float GetStrikeZoneHeight()
	{
		return pStrikeZoneData_->height;
	}

	//ストライクゾーンの中心位置を取得
	XMVECTOR GetStrikeZoneCenter()
	{
		return pStrikeZoneData_->center;
	}

	//ボールの半径を取得
	float GetBallRadius()
	{
		return pBallData_->radius;
	}

	//ボールの重さを取得
	float GetBallWeight()
	{
		return pBallData_->weight;
	}

	//バットの半径を取得
	float GetBatRadius()
	{
		return pBatterData_->batRadius;
	}

	//バッターのパワーを取得
	float GetBatterPower()
	{
		return pBatterData_->power;
	}

	//バッターのの力が最も加わるフレーム数を取得
	int GetBatterImpactFrame()
	{
		return pBatterData_->impactFrame;
	}

	//ピッチャーが扱う球種の数を取得
	int GetBallTypeCount()
	{
		return (int)pPitcherData_->ballTypeDataList.size();
	}

	//ピッチャーがボールを放すフレーム数を取得
	int GetReleaseFrame()
	{
		return pPitcherData_->releaseFrame;
	}

	//残り回数を取得
	int GetRemainingCount()
	{
		return pGameModeData_->remainingCount;
	}

	//ゲーム内の動きの速度を取得
	float GetGameSpeedAcceleration()
	{
		return pGameModeData_->gameSpeedAcceleration;
	}

	//ピッチャーがどこまでコースを狙うかを取得
	float GetCoursePercentage()
	{
		return pGameModeData_->coursePercentage;
	}

	//最大プレイ回数を取得
	int GetMaxRemainingCount()
	{
		return pGameModeData_->remainingCount;
	}

	//ホームプレートの位置を取得
	XMVECTOR GetHomePlatePosition()
	{
		return pFieldData_->homePlatePosition;
	}

	//ピッチャープレートの位置を取得
	XMVECTOR GetPitcherPlatePosition()
	{
		return pFieldData_->pitcherPlatePosition;
	}

	//ホームランになる距離を取得
	float GetHomeRunDistance()
	{
		return pFieldData_->homeRunDistance;
	}

	//タイトルシーンで使用する画像データへのファイルパスの読み込み
	void LoadTitleSceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																//データがある列

		//タイトルシーンで使用する画像データへのファイルパスが入っているCSVファイルの読み込み
		Csv titleSceneImageFilePathData;														//タイトルシーンで使用する画像データへのファイルパスが入っているCSVファイルデータ
		assert(titleSceneImageFilePathData.Load(fileName));										//CSVファイルの読み込み

		//シーンで使用する画像へのファイルパスデータをクリアする
		sceneImageFilePathList_.clear();

		//シーンで使用する画像へのファイルパスデータを格納
		for (int row = 0; row < (int)TITLE_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = titleSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//画像へのファイルパス
			sceneImageFilePathList_.push_back(filePath);										//リストに格納
		}
	}

	//プレイシーンで使用する画像データへのファイルパスの読み込み
	void LoadPlaySceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																//データがある列

		//プレイシーンで使用する画像データへのファイルパスが入っているCSVファイルの読み込み
		Csv playSceneImageFilePathData;															//プレイシーンで使用する画像データへのファイルパスが入っているCSVファイルデータ
		assert(playSceneImageFilePathData.Load(fileName));										//CSVファイルの読み込み

		//シーンで使用する画像へのファイルパスデータをクリアする
		sceneImageFilePathList_.clear();

		//シーンで使用する画像へのファイルパスデータを格納
		for (int row = 0; row < (int)PLAY_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = playSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//画像へのファイルパス
			sceneImageFilePathList_.push_back(filePath);										//リストに格納
		}

		const int ROW_NUMBER_FILE_PATH						= (int)PLAY_SCENE_IMAGE::MAX;																						//数字の画像へのファイルパスが格納されている行
		numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH]	= playSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::FILE_PATH);	//数字の画像があるファイルまでのファイルパス
		numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION]	= playSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::EXTENSION);	//数字の画像の拡張子
	}

	//リザルトシーンで使用する画像データへのファイルパスの読み込み
	void LoadResultSceneImageFilePath(const std::string& fileName)
	{
		const int COLUMN_DATA = 1;																	//データがある列

		//リザルトシーンで使用する画像データへのファイルパスが入っているCSVファイルの読み込み
		Csv resultSceneImageFilePathData;															//リザルトシーンで使用する画像データへのファイルパスが入っているCSVファイルデータ
		assert(resultSceneImageFilePathData.Load(fileName));										//CSVファイルの読み込み

		//シーンで使用する画像へのファイルパスデータをクリアする
		sceneImageFilePathList_.clear();

		//シーンで使用する画像へのファイルパスデータを格納
		for (int row = 0; row < (int)RESULT_SCENE_IMAGE::MAX; row++)
		{
			std::string filePath = resultSceneImageFilePathData.GetStringDeta(row, COLUMN_DATA);	//画像へのファイルパス
			sceneImageFilePathList_.push_back(filePath);											//リストに格納
		}

		const int ROW_NUMBER_FILE_PATH						= (int)RESULT_SCENE_IMAGE::MAX;																						//数字の画像へのファイルパスが格納されている行
		numberFilePath_[(int)NUMBER_FILE_PATH::FILE_PATH]	= resultSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::FILE_PATH);	//数字の画像があるファイルまでのファイルパス
		numberFilePath_[(int)NUMBER_FILE_PATH::EXTENSION]	= resultSceneImageFilePathData.GetStringDeta(ROW_NUMBER_FILE_PATH, COLUMN_DATA + (int)NUMBER_FILE_PATH::EXTENSION);	//数字の画像の拡張子
	}

	//係数値データの読み込み
	void LoadCoefficient(const std::string& objectName, std::string const& fileName)
	{
		bool IS_EXISTS = objectCoefficientMap_.find(objectName) != objectCoefficientMap_.end();			//objectNameのデータが存在するか
		if (IS_EXISTS) return;																			//ロード済みなら何もしない

		enum class ROW 
		{
			RESTITUTION = 0,																			//反発係数がある行
			FRICTION,																					//摩擦係数がある行
			MAX,
		};
		const int COLUMN_DATA = 1;																		//データがある列

		//係数値データが入っているCSVファイルの読み込み
		Csv coefficientData;																			//係数値が入っているCSVファイルデータ
		assert(coefficientData.Load(fileName));															//CSVファイルの読み込み

		//新しい係数値データの作成
		Coefficient* pCoefficient	= new Coefficient;													//新しい係数値
		pCoefficient->restitution	= coefficientData.GetFloatDeta((int)ROW::RESTITUTION, COLUMN_DATA);	//反発係数値を格納
		pCoefficient->friction		= coefficientData.GetFloatDeta((int)ROW::FRICTION, COLUMN_DATA);	//摩擦係数値を格納

		objectCoefficientMap_.emplace(objectName, pCoefficient);										//係数値クラスをを格納
	}

	//ストライクゾーンのデータの読み込み
	void LoadStrikeZoneData(const std::string& fileName)
	{
		//行ごとに入っているデータ
		enum class ROW 
		{
			FILE_PATH = 0,														//モデルデータへのファイルパスがある行
			POSITION,															//位置がある行
			WIDTH,																//横幅の値がある行
			MAX,
		};

		const int COLUMN_DATA = 1;												//データがある列

		//ストライクゾーンのデータが入っているCSVファイルの読み込み
		Csv strikeZoneData;														//ストライクゾーンのデータが入っているCSVファイルデータ
		assert(strikeZoneData.Load(fileName));									//CSVファイルの読み込み
			
		//ストライクゾーンの用のメモリがなければ新しく確保
		if (pStrikeZoneData_ == nullptr) pStrikeZoneData_ = new StrikeZoneData;				

		//ストライクゾーンのデータを格納
		pStrikeZoneData_->filePath	= strikeZoneData.GetStringDeta((int)ROW::FILE_PATH, COLUMN_DATA);		//モデルデータのファイルパス
		pStrikeZoneData_->center	= GetVector3CsvData(strikeZoneData, (int)ROW::POSITION, COLUMN_DATA);	//中心座標
		pStrikeZoneData_->width		= strikeZoneData.GetFloatDeta((int)ROW::WIDTH, COLUMN_DATA);			//幅
	}

	//ボールのデータの読み込み
	void LoadBallData(const std::string& fileName)
	{
		//行ごとに入っているデータ
		enum class ROW 
		{
			FILE_PATH = 0,																	//モデルデータへのファイルパスがある行
			RADIUS,																			//半径の値がある行
			WEIGHT,																			//重さの値がある行
			MAX,
		};

		const int COLUMN_DATA = 1;															//データがある列

		//ボールのデータが入っているCSVファイルの読み込み
		Csv ballData;																		//ボールのデータが入っているCSVファイルデータ
		assert(ballData.Load(fileName));													//CSVファイルの読み込み

		//ボールデータ用のメモリがなければ新しく確保
		if (pBallData_ == nullptr) pBallData_ = new BallData;				

		//ボールのデータを格納
		pBallData_->filePath	= ballData.GetStringDeta((int)ROW::FILE_PATH, COLUMN_DATA);	//モデルデータのファイルパス
		pBallData_->radius		= ballData.GetFloatDeta((int)ROW::RADIUS, COLUMN_DATA);		//半径
		pBallData_->weight		= ballData.GetFloatDeta((int)ROW::WEIGHT, COLUMN_DATA);		//重さ
	}

	//バッターのデータの読み込み
	void LoadBatterData(const std::string& fileName)
	{
		//行ごとに入っているデータ
		enum class ROW 
		{
			FILE_PATH = 0,															//モデルデータへのファイルパスがある行
			RADIUS = (int)SWING_COURSE::MAX,										//半径の値がある行
			POWER,																	//パワーの値がある行
			IMPACT_FRAME,															//最も力が加わるフレーム数
			MAX,
		};

		const int COLUMN_DATA = 1;													//データがある列

		//バッターのデータが入っているCSVファイルの読み込み
		Csv batterData;																//バッターのデータが入っているCSVファイルデータ
		assert(batterData.Load(fileName));											//CSVファイルの読み込み

		//バッターデータ用のメモリがなければ新しく確保
		if (pBatterData_ == nullptr) pBatterData_ = new BatterData;					

		//バッターのデータを格納
		//アニメーションのスイングするコースごとのモデルデータのファイルパスを格納
		for (int i = 0; i < (int)SWING_COURSE::MAX; i++)
			pBatterData_->filePath[i]	= batterData.GetStringDeta((int)ROW::FILE_PATH + i, COLUMN_DATA);

		pBatterData_->batRadius			= batterData.GetFloatDeta((int)ROW::RADIUS, COLUMN_DATA);			//バットの半径
		pBatterData_->power				= batterData.GetFloatDeta((int)ROW::POWER, COLUMN_DATA);			//パワー値
		pBatterData_->impactFrame		= batterData.GetIntegerDeta((int)ROW::IMPACT_FRAME, COLUMN_DATA);	//最も力が加わるフレーム数
	}

	//ピッチャーのデータの読み込み
	void LoadPitcherData(const std::string& fileName)
	{
		//行ごとに入っているデータ
		enum class ROW 
		{
			FILE_PATH			= 0,										//モデルデータへのファイルパスがある行
			RELEASE_FRAME,													//ボールを放すフレーム数
			BALL_TYPE			= 4,										//球種
			MAX,
		};

		//列ごとに入っているデータ
		enum class COLUMN
		{
			PITCHER_DATA		= 1,										//何フレーム目でボールを放すか
			BALL_TYPE_NAME		= 0,										//球種名
			THROW_PROBABILITY,												//投げる割合
			MAX,
		};

		//ピッチャーのデータが入っているCSVファイルの読み込み
		Csv pitcherData;													//ピッチャーのデータが入っているCSVファイルデータ
		assert(pitcherData.Load(fileName));									//CSVファイルの読み込み

		//ピッチャーデータ用のメモリがなければ新しく確保
		if (pPitcherData_ == nullptr) pPitcherData_ = new PitcherData;

		//データの格納
		pPitcherData_->filePath		= pitcherData.GetStringDeta((int)ROW::FILE_PATH, (int)COLUMN::PITCHER_DATA);		//モデルデータのファイルパス
		pPitcherData_->releaseFrame = pitcherData.GetIntegerDeta((int)ROW::RELEASE_FRAME, (int)COLUMN::PITCHER_DATA);	//ボールを放すフレーム数を格納

		//ピッチャーが使用する球種データの読み込み
		const int ROW_END = (int)pitcherData.GetHeight();					//最後の行
		for (int row = (int)ROW::BALL_TYPE; row < (int)ROW_END; row++)
		{
			const std::string	BALL_TYPE_NAME		= pitcherData.GetStringDeta(row, (int)COLUMN::BALL_TYPE_NAME);		//球種名
			const float			THROW_PROBABILITY	= pitcherData.GetFloatDeta(row, (int)COLUMN::THROW_PROBABILITY);	//BALL_TYPE_NAMEを使用する割合

			//球種ごとのデータをロード
			LoadBallTypeData("Data/BallType/" + BALL_TYPE_NAME + ".csv");

			//ロードした球種を使用する割合を格納
			pPitcherData_->throwProbabilityList.push_back(THROW_PROBABILITY);
		}
	}

	//変化球のデータの読み込み
	void LoadBallTypeData(const std::string& fileName)
	{
		//列ごとに入っているデータの内容
		enum class ROW
		{
			SPEED = 0,											//球速
			ROTATE,												//回転数
			ROTATE_AXIS,										//回転軸
			MAX,
		};

		const int COLUMN_DATA = 1;								//データがある列

		//球種ごとのデータが入っているCSVファイルの読み込み
		Csv ballTypeData;										//球種のデータが入っているCSVファイルデータ
		assert(ballTypeData.Load(fileName));					//CSVファイルの読み込み

		//新しい球種データを作成
		BallTypeData* newBallTypeData = new BallTypeData;

		//各データを取得
		newBallTypeData->speed				= ballTypeData.GetFloatDeta((int)ROW::SPEED, COLUMN_DATA);					//球速を格納
		newBallTypeData->rotate				= ballTypeData.GetFloatDeta((int)ROW::ROTATE, COLUMN_DATA);					//回転数を格納
		newBallTypeData->rotateAxis			= GetVector3CsvData(ballTypeData, (int)ROW::ROTATE_AXIS, COLUMN_DATA);		//回転軸のベクトル格納

		//向きベクトルを正規化して単位ベクトルに変換
		newBallTypeData->rotateAxis			= DirectX::XMVector3Normalize(newBallTypeData->rotateAxis);

		//ピッチャーの持ち球種として格納
		pPitcherData_->ballTypeDataList.push_back(newBallTypeData);
	}

	//ゲームモードで変動するゲーム内容に関するデータの読み込み
	void LoadGameModeData(const std::string& fileName)
	{
		const int	ROW_DATA_BEGIN	= 1;
		const int	ROW_GAME_MODE	= (int)InternalPlayDataReservoir::GetCurrentGameMode() + ROW_DATA_BEGIN;	//データがある行
		
		//列ごとに入っているデータの内容
		enum class COLUMN
		{
			GAME_SPEED_ACCELERATION = 1,										//ゲームスピード
			REMAINING_COUNT,													//残り回数数
			COURSE_PERCENTAGE,													//ピッチャーがどれだけコースをつけるか
			MAX,
		};

		//ゲームモードに関するデータが入っているCSVファイルの読み込み
		Csv gameModeData;														//ゲームモードのデータが入っているCSVファイルデータ
		assert(gameModeData.Load(fileName));									//CSVファイルの読み込み

		//ゲームモードに関するデータ用のメモリがなければ新しく確保
		if (pGameModeData_ == nullptr) pGameModeData_ = new GameModeData;

		//ゲームモードに合わせたデータの取得
		pGameModeData_->gameSpeedAcceleration	= gameModeData.GetFloatDeta(ROW_GAME_MODE, (int)COLUMN::GAME_SPEED_ACCELERATION);	//ゲームスピード
		pGameModeData_->remainingCount			= gameModeData.GetIntegerDeta(ROW_GAME_MODE, (int)COLUMN::REMAINING_COUNT);			//残り回数数
		pGameModeData_->coursePercentage		= gameModeData.GetFloatDeta(ROW_GAME_MODE, (int)COLUMN::COURSE_PERCENTAGE);			//ピッチャーがどこまでコースを狙えるか
	}

	//フィールドのデータの読み込み
	void LoadFieldData(const std::string& fileName)
	{
		//行ごとに入っているデータ
		enum class ROW
		{
			FILE_PATH_FIELD_OBJECT = 0,											//フィールドに置かれるモデルデータへのファイルパスがある行
			FILE_PATH_GROUND,													//地面のモデルデータへのファイルパスがある行
			PITCHER_PLATE_POSITION,												//ピッチャープレートの位置がある行
			HOME_PLATE_POSITION,												//ホームプレートの位置がある行
			HOMERUN_DISTANCE,													//ホームランになる距離がある行
			MAX,
		};

		const int COLUMN_DATA = 1;												//データがある列

		//バッターのデータが入っているCSVファイルの読み込み
		Csv fieldData;															//バッターのデータが入っているCSVファイルデータ
		assert(fieldData.Load(fileName));										//CSVファイルの読み込み

		//バッターデータ用のメモリがなければ新しく確保
		if (pFieldData_ == nullptr) pFieldData_ = new FieldData;

		//フィールドのデータを格納
		pFieldData_->filePath[(int)FIELD_MODEL::FIELD_OBUJECT]	= fieldData.GetStringDeta((int)ROW::FILE_PATH_FIELD_OBJECT, COLUMN_DATA);		//フィールドに置かれるモデルデータのファイルパス
		pFieldData_->filePath[(int)FIELD_MODEL::GROUND]			= fieldData.GetStringDeta((int)ROW::FILE_PATH_GROUND, COLUMN_DATA);				//地面のモデルデータのファイルパス
		pFieldData_->pitcherPlatePosition						= GetVector3CsvData(fieldData, (int)ROW::PITCHER_PLATE_POSITION, COLUMN_DATA);	//ピッチャープレートの位置
		pFieldData_->homePlatePosition							= GetVector3CsvData(fieldData, (int)ROW::HOME_PLATE_POSITION, COLUMN_DATA);		//ホームプレートの位置
		pFieldData_->homeRunDistance							= fieldData.GetFloatDeta((int)ROW::HOMERUN_DISTANCE, COLUMN_DATA);				//ホームプレートの位置
	}

	//背景描画用のデータを読み込み
	void LoadBackGroundData(const std::string& fileName)
	{
		const int	ROW_DATA_FIRST = 1;											//データがある最初の行

		//列ごとに入っているデータの内容
		enum class COLUMN
		{
			FILE_PATH = 0,														//モデルデータへのファイルパスがある行
			SHADER_TYPE,														//使用するシェーダ
			MAX,
		};

		//背景に関するデータが入っているCSVファイルの読み込み
		Csv backGroundData;														//背景のデータが入っているCSVファイルデータ
		assert(backGroundData.Load(fileName));									//CSVファイルの読み込み

		const int ROW_DATA_END = (int)backGroundData.GetHeight();				//データがある最後の行

		//ゲームモードに関するデータ用のメモリがなければ新しく確保
		backGroundDataList_.clear();

		//背景データを一行分ずつリストに追加
		int count = 0;
		for (int row = ROW_DATA_FIRST; row < ROW_DATA_END; row++)
		{
			BackGroundData newData;				//新規データ

			//背景データの取得
			newData.filePath	= backGroundData.GetStringDeta(row, (int)COLUMN::FILE_PATH);		//ファイルパス
			newData.shaderType	= backGroundData.GetIntegerDeta(row, (int)COLUMN::SHADER_TYPE);		//シェーダ
			
			//リストに追加
			backGroundDataList_.push_back(newData);
		}
	}

	//csvファイルに同じ行で3連続で格納されている値をベクトルのx,y,zとして取得する
	XMVECTOR GetVector3CsvData(Csv& csv, int row, int column)
	{
		//ベクトル要素
		enum class VECTOR
		{
			X = 0,	//X
			Y,		//Y
			Z,		//Z
			MAX,
		};

		//各要素の値を取得する
		XMVECTOR out	= DirectX::g_XMZero;									//出力する値
		out.vecX		= csv.GetFloatDeta(row, column + (int)VECTOR::X);
		out.vecY		= csv.GetFloatDeta(row, column + (int)VECTOR::Y);
		out.vecZ		= csv.GetFloatDeta(row, column + (int)VECTOR::Z);

		return out;
	}

	//解放
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