#include "HomeRunImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

HomeRunImage::HomeRunImage() : HomeRunImage(nullptr)
{
}

HomeRunImage::HomeRunImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//初期化
void HomeRunImage::Initialize()
{
	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::HOME_RUN);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0, 0.2f, 0, 0);			//描画位置
	transform_.scale_		= DirectX::XMVectorSet(1.5f, 1.5f, 0, 0);		//拡大サイズ
}

//更新
void HomeRunImage::Update()
{
}

//描画
void HomeRunImage::Draw()
{	
	if (pPlayScene_ == nullptr)		return;							//プレイシーンのポインタが存在しなければ描画しない
	if (!pPlayScene_->IsInPlay())	return;							//プレー中でなければ描画しない
	if (!pPlayScene_->IsHomeRun())	return;							//ホームランでなければ描画しない

	//文字画像の描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::HOME_RUN);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void HomeRunImage::Release()
{
}