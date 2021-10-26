#pragma once
#include <string>
#include "Engine/Transform.h"
#include "Engine/Csv.h"

//外部ファイルのデータを管理
namespace ExternalDataManager
{
	using DirectX::XMVECTOR;

	//難易度モード
	enum class GAME_MODE
	{
		EASY = 0,		//易しい
		NORMAL,			//普通
		HARD,			//難しい
		MAX,
	};

	//スイングのコース分け
	enum class SWING_COURSE
	{
		LEFT_HIGH = 0,	//左上
		LEFT_LOW,		//左下
		RIGHT_HIGH,		//右上
		RIGHT_LOW,		//右下
		CENTER,			//中心
		MAX,
	};

	//タイトルシーンで使用する画像の種類
	enum class TITLE_SCENE_IMAGE
	{
		TITLE = 0,		//タイトル
		MODE_EASY,		//難易度選択（易しい）
		MODE_NORMAL,	//難易度選択（普通）
		MODE_HARD,		//難易度選択（難しい）
		MAX,
	};

	//プレイシーンで使用する画像の種類
	enum class PLAY_SCENE_IMAGE
	{
		SCORE_BOARD = 0,//黒板
		PURPOSE,		//目的表示に使用する文字列画像
		REMAINING,		//残りの球数表示に使用する文字列画像
		SCORE,			//スコア表示に使用する文字列画像
		HOME_RUN,		//ホームランであることを表すのに使用する文字列画像
		MAX,
	};

	//リザルトシーンで使用する画像の種類
	enum class RESULT_SCENE_IMAGE
	{
		SCORE_BOARD = 0,//黒板
		SCORE,			//スコア表示に使用する文字列画像
		END_GAME,		//ゲームを終了する
		TITLE_BACK,		//タイトルに戻る
		MAX,
	};

	const int DECIMAL_BASE = 10;		//10進数の底

	////////////////////
	//シーンごとに使用するデータの初期化関数
	void InitializeTitleSceneData();	//タイトルシーン
	void InitializePlaySceneData();		//プレイシーン
	void InitializeResultSceneData();	//リザルトシーン


	////////////////////
	//設定関数

	//ストライクゾーンの設定
	//引数１：height		ストライクゾーンの高さ
	//引数２：center		ストライクゾーンの中心位置
	void SetStrikeZoneHeight(float height, float center);

	//フィールドデータの設定
	//引数１：homePlatePosition		ホームプレートの位置
	//引数２：pitcherPlatePosition	ピッチャープレートの位置
	//引数３：homeRunDistance		ホームランになる距離
	void SetFieldData(const XMVECTOR& homePlatePosition, const XMVECTOR& pitcherPlatePosition, float homeRunDistance);

	////////////////////
	//取得関数

	//シーンで使用する画像データへのファイルパスを取得
	//引数：image			画像の種類
	std::string GetFilePathTitleSceneImage(TITLE_SCENE_IMAGE image);		//タイトルシーン
	std::string GetFilePathPlaySceneImage(PLAY_SCENE_IMAGE image);			//プレイシーン
	std::string GetFilePathResultSceneImage(RESULT_SCENE_IMAGE image);		//リザルトシーン

	//数字の画像データへのファイルパスを取得
	//引数：number			画像の数字
	std::string GetFilePathNumberImage(int number);

	//係数値の取得
	//引数：objectName		取得する値がなんのオブジェクトのものか
	float		GetCoefficientRestitution(const std::string& objectName);	//反発係数
	float		GetCoefficientFriction(const std::string& objectName);		//摩擦係数
	
	//球種ごとの値を取得
	//引数：type			何個目の球種か
	float		GetUseRateBallType(int type);								//使用率					
	float		GetBallSpeed(int type);										//球速
	float		GetRotate(int type);										//回転数
	XMVECTOR	GetRotateAxis(int type);									//回転軸

	//バッターのモデルデータへのファイルパスを取得
	//引数：course			モデルのアニメーションがスイングするコース
	std::string GetFilePathBatterModel(SWING_COURSE course);

	//背景データの取得
	std::string GetFilePathBackGroundModel(int index);						//背景のモデルデータへのファイルパス
	int			GetShaderTypeBackGround(int index);							//使用するシェーダ
	
	//無引数の取得関数
	std::string GetFilePathPitcherModel();									//ピッチャーのモデルデータへのファイルパス
	std::string GetFilePathBallModel();										//ボールのモデルデータへのファイルパス
	std::string GetFilePathStrikeZoneModel();								//ストライクゾーンのモデルデータへのファイルパス
	std::string GetFilePathFieldObjectModel();								//フィールドに置くオブジェクトのモデルデータへのファイルパス
	std::string GetFilePathGroundModel();									//地面のモデルデータへのファイルパスを取得
	float		GetStrikeZoneWidth();										//ストライクゾーンの幅
	float		GetStrikeZoneHeight();										//ストライクゾーンの高さ
	XMVECTOR	GetStrikeZoneCenter();										//ストライクゾーンの中心位置
	float		GetBallRadius();											//ボールの半径
	float		GetBallWeight();											//ボールの重さ
	float		GetBatRadius();												//バットの半径
	float		GetBatterPower();											//バッターのパワー
	int			GetBatterImpactFrame();										//バッターのの力が最も加わるフレーム数
	int			GetBallTypeCount();											//ピッチャーが扱う球種の数
	int			GetReleaseFrame();											//ピッチャーがボールを放すフレーム数
	float		GetGameSpeedAcceleration();									//ゲーム内の動きの速度
	float		GetCoursePercentage();										//ピッチャーがどこまでコースを狙うか
	int			GetMaxRemainingCount();										//最大プレイ回数
	XMVECTOR	GetHomePlatePosition();										//ホームプレートの位置
	XMVECTOR	GetPitcherPlatePosition();									//ピッチャープレートの位置
	float		GetHomeRunDistance();										//ホームランになる距離

	

	////////////////////
	//読み込み関数

	//オブジェクト名をキーにしてオブジェクトごとの係数値を格納
	//引数１：objectName	読み込むデータのオブジェクト名
	//引数２：fileName		読み込むファイル名（パス）
	void LoadCoefficient(const std::string& objectName, std::string const & fileName);

	//ファイルから値を読み込む
	//引数：fileName		読み込むファイル名（パス）
	void LoadTitleSceneImageFilePath(const std::string& fileName);			//タイトルシーンで使用する画像データへのファイルパス
	void LoadPlaySceneImageFilePath(const std::string& fileName);			//プレイシーンで使用する画像データへのファイルパス
	void LoadResultSceneImageFilePath(const std::string& fileName);			//リザルトシーンで使用する画像データへのファイルパス
	void LoadStrikeZoneData(const std::string& fileName);					//ストライクゾーンのデータ
	void LoadBallData(const std::string& fileName);						//ボールのデータ
	void LoadBatterData(const std::string& fileName);						//バッターのデータ
	void LoadPitcherData(const std::string& fileName);						//ピッチャーのデータ
	void LoadBallTypeData(const std::string& fileName);					//変化球のデータ
	void LoadGameModeData(const std::string& fileName);					//ゲームモードで変動するゲーム内容に関するデータ
	void LoadFieldData(const std::string& fileName);						//フィールドのデータ
	void LoadBackGroundData(const std::string& fileName);					//背景描画用のデータ


	//csvファイルに同じ行で3連続で格納されている値をベクトルのx,y,zとして取得する
	//引数１：csv	読み込むデータのオブジェクト名
	//引数２：row		取得するデータのある行
	//引数３：column	取得するデータのある列
	//戻値	：csvから読み取ったベクトル値
	XMVECTOR GetVector3CsvData(Csv& csv, int row, int column);

	//解放
	void Release();
};