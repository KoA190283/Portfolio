#include "TitleImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

TitleImage::TitleImage()
{
}

//初期化
void TitleImage::Initialize()
{
	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::TITLE);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_ = DirectX::XMVectorSet(0, 0.4f, 0, 0);		
}

//更新
void TitleImage::Update()
{
}

//描画
void TitleImage::Draw()
{
	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::TITLE);	//タイトル画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//タイトル画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void TitleImage::Release()
{
}