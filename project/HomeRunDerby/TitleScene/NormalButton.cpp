#include "NormalButton.h"

#include "../Engine/Image.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"

NormalButton::NormalButton() :
	NormalButton(nullptr)
{
}

NormalButton::NormalButton(TitleScene* pTitleScene)
{
	pTitleScene_ = pTitleScene;
}

//初期化
void NormalButton::Initialize()
{
	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_NORMAL);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0    , -0.4f, 0, 0);
	transform_.scale_		= DirectX::XMVectorSet(0.85f, 0.85f, 0, 0);
}

//更新
void NormalButton::Update()
{
	if (pTitleScene_ == nullptr) return;	//タイトルシーンのポインタが無ければ更新処理を行わない

	//クリックされているかを確認し、されていなければ更新処理を行わない
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_NORMAL);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	if (!Image::IsClicked(IMAGE_HANDLE))	return;

	//ゲームモードの設定
	InternalPlayDataReservoir::SetGameMode(ExternalDataManager::GAME_MODE::NORMAL);

	//次のシーンに変更
	pTitleScene_->ChangeNextScene();
}

//描画
void NormalButton::Draw()
{
	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_NORMAL);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void NormalButton::Release()
{
}
