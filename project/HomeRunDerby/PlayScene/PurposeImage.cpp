#include "PurposeImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

PurposeImage::PurposeImage() : PurposeImage(nullptr)
{
}

PurposeImage::PurposeImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//初期化
void PurposeImage::Initialize()
{
	//文字の画像データの読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::PURPOSE);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0.55f, 0.75f, 0, 0);	//描画位置
	transform_.scale_		= DirectX::XMVectorSet(0.5f, 0.5f, 0, 0);	//拡大サイズ
}

//更新
void PurposeImage::Update()
{
}

//描画
void PurposeImage::Draw()
{
	if (pPlayScene_ == nullptr)		return;	//プレイシーンのポインタが存在しなければ描画しない
	if (pPlayScene_->IsInPlay())	return;	//プレー中であれば描画しない

	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::PURPOSE);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void PurposeImage::Release()
{
}