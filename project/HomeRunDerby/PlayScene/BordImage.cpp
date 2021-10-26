#include "BordImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

BordImage::BordImage() : BordImage(nullptr)
{
}

BordImage::BordImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//初期化
void BordImage::Initialize()
{
	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::SCORE_BOARD);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0.55f, -0.55f, 0.0001f, 0);	//描画位置
}

//更新
void BordImage::Update()
{
}

//描画
void BordImage::Draw()
{
	if (pPlayScene_ == nullptr)		return;	//プレイシーンのポインタが存在しなければ描画しない
	if (pPlayScene_->IsInPlay())	return;	//プレー中であれば描画しない

	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::SCORE_BOARD);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void BordImage::Release()
{
}