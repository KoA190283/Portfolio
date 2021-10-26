#include "../Engine/Global.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Input.h"
#include "../Engine/Camera.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"
#include "Ball.h"
#include "Batter.h"
#include "BordImage.h"
#include "Field.h"
#include "HomeRunImage.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "PurposeImage.h"
#include "RemainingCountImage.h"
#include "ScoreImage.h"
#include "StrikeZone.h"

using XMVECTOR = DirectX::XMVECTOR;

//コンストラクタ
PlayScene::PlayScene(IGameObject* parent)
	: IGameObject(parent, "PlayScene"), pUmpire_(nullptr), timer_(0)
{
}

//初期化
void PlayScene::Initialize()
{
	//プレイシーンで使用するデータの初期化
	ExternalDataManager::InitializePlaySceneData();
	InternalPlayDataReservoir::InitializePlayData();

	//審判の初期化
	pUmpire_ = new Umpire;

	//背景の初期化
	BackGround::Initialize();

	//プレイシーン内のオブジェクトのインスタンス化
	Instantiate<Field>(this);
	Instantiate<Ball>(this);
	Instantiate<Batter>(this);
	Instantiate<Pitcher>(this);
	Instantiate<StrikeZone>(this);

	//UIのインスタンスを生成しリストに追加
	userInterfaceList_.push_back(new BordImage(this));
	userInterfaceList_.push_back(new HomeRunImage(this));
	userInterfaceList_.push_back(new PurposeImage(this));
	userInterfaceList_.push_back(new RemainingCountImage(this));
	userInterfaceList_.push_back(new ScoreImage(this));

	//UIの初期化
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}

	//カメラを初期位置に設定する
	ResetCamera();
}

//更新
void PlayScene::Update()
{
	using DirectX::operator-;

	//ゲームの終了フラグがtrueならリザルトシーンに切り替える
	if (pUmpire_->IsEndGame())
	{
		SceneManager* sceneManager = (SceneManager*)pParent_;
		sceneManager->ChangeScene(SCENE_ID::RESULT);
		return;
	}

	//UIの更新
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
	
	//進行状況を更新
	UpdateProgress();

	//ボールがバットに当たっていたらカメラはボールを追う
	if (pUmpire_->IsHitBall())
	{
		//ボールの位置を取得
		Ball* pBall = (Ball*)FindChildObject("Ball");
		const XMVECTOR BALL_POSITION = pBall->GetPosition();
		
		//飛距離が一定以上あったらカメラの位置を変更する
		const float DISTANCE_LIMIT			= 50.f;
		const float DISTENCE_HOME_TO_BALL	= DirectX::XMVector3Length(BALL_POSITION - ExternalDataManager::GetHomePlatePosition()).vecX;
		if (DISTENCE_HOME_TO_BALL > DISTANCE_LIMIT)
		{
			XMVECTOR newCameraPosition;
			newCameraPosition.vecX = 0.f;		
			newCameraPosition.vecY = 20.f;			
			newCameraPosition.vecZ = -70.f;			

			Camera::SetPosition(newCameraPosition);
		}

		//カメラのターゲットをボールの現在の位置に変更
		Camera::SetTarget(BALL_POSITION);
	}
}

//描画
void PlayScene::Draw()
{
	//背景描画
	BackGround::Draw();
}

//解放
void PlayScene::Release()
{
	SAFE_DELETE(pUmpire_);
	
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		SAFE_RELEASE(pUserInterface);
	}
	userInterfaceList_.clear();

	ExternalDataManager::Release();
}

//子孫の描画を行った後に行う描画処理
void PlayScene::DrawAfterChildren()
{	
	//UIの描画
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Draw();
	}
}

//ボールが何かと衝突したときの処理
void PlayScene::CollisionBall(const XMVECTOR & collisionPosision, const std::string & objectName)
{
	if (!pUmpire_->IsInPlay())	return;	//プレー中でなければ何もしない

	//ボールと当たったオブジェクトがバッターだった場合の処理
	if (objectName == "Batter")
	{
		//ストライクゾーンの更新と描画を無効化する
		StrikeZone* pStrikeZone = (StrikeZone*)FindChildObject("StrikeZone");
		pStrikeZone->Inactivate();
		pStrikeZone->Invisible();
	}
	
	//衝突したオブジェクト名がフィールドでフライの状態であればタイマーを0にする
	if (objectName == "Field")
		if (!pUmpire_->IsGroundBall())	timer_ = 0;

	//ボールが何かと衝突したときの処理を行う
	pUmpire_->CollisionBall(collisionPosision, objectName);
}

//ゲーム内の速度を取得
float PlayScene::GetGameSpeedAcceleration() const
{
	//バットとボールが当たっていたら現実より少し早くする
	if (pUmpire_->IsHitBall())	return 2.0f;

	//当たっていなければ調整された速度
	else						return ExternalDataManager::GetGameSpeedAcceleration();
}

//プレー中であるかを取得
bool PlayScene::IsInPlay() const
{
	return pUmpire_->IsInPlay();
}

//打球かを取得
bool PlayScene::IsHitBall() const
{
	return pUmpire_->IsHitBall();
}

//ホームランかを取得
bool PlayScene::IsHomeRun() const
{
	return pUmpire_->IsHomeRun();
}

//進行状況を更新
void PlayScene::UpdateProgress()
{
	timer_++;

	//プレー中の処理
	if (pUmpire_->IsInPlay())
	{
		//地面にボールがついている場合の処理
		if (pUmpire_->IsGroundBall())
		{
			//ファウルボールならプレーを終了する
			if (pUmpire_->IsFoulBall())	EndPlay();

			//一定時間が経過していればプレーを終了する
			const int END_PLAY_TIME = 120;			//地面にボールがついてからプレーを終了するまでの時間
			if (timer_ > END_PLAY_TIME)	EndPlay();
		}
	}
	//プレー中ではない場合の処理
	else
	{
		//開始時間になったらプレーを開始する
		const int BEGIN_PLAY_TIME = 120;		//プレーしていない時間
		if (timer_ > BEGIN_PLAY_TIME)	BeginPlay();
	}
}

//プレー開始
void PlayScene::BeginPlay()
{
	//ピッチャーの投球の準備をする
	Pitcher* pPitcher = (Pitcher*)FindChildObject("Pitcher");
	pPitcher->InitializePitch();

	//ボールの描画を有効化
	Ball* pBall = (Ball*)FindChildObject("Ball");
	pBall->Visible();
	pBall->Inactivate();

	//プレーを開始する
	pUmpire_->BeginPlay();
}

//プレー終了
void PlayScene::EndPlay()
{
	//プレーを終了する
	pUmpire_->EndPlay();

	//残りの球数が0ならゲームの終了フラグをtrueにする
	if (InternalPlayDataReservoir::GetRemainingCount() == 0)
	{
		pUmpire_->EndGame();
		return;
	}

	//ボールの更新と描画を無効化
	Ball* pBall = (Ball*)FindChildObject("Ball");
	pBall->Inactivate();
	pBall->Invisible();

	//バッターのアニメーションを最初の状態に戻す
	Batter* pBatter = (Batter*)FindChildObject("Batter");
	pBatter->ResetAnimation();

	//ピッチャーのアニメーションを最初の状態に戻す
	Pitcher* pPitcher = (Pitcher*)FindChildObject("Pitcher");
	pPitcher->ResetAnimation();

	//ストライクゾーンの更新と描画を有効化する
	StrikeZone* pStrikeZone = (StrikeZone*)FindChildObject("StrikeZone");
	pStrikeZone->Activate();
	pStrikeZone->Visible();

	//カメラを初期位置に戻す
	ResetCamera();

	//タイマーを0にする
	timer_ = 0;
}

//カメラを最初の位置に戻す
void PlayScene::ResetCamera() const
{
	Camera::SetPosition(0.f, 1.2f, 4.f);	//位置
	Camera::SetTarget(0, 1.f, -1.f);		//見る位置
}
