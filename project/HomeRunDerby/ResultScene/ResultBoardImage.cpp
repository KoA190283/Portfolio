#include "ResultBoardImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

ResultBoardImage::ResultBoardImage()
{
}

//初期化
void ResultBoardImage::Initialize()
{
	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE_BOARD);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																					//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0.f,  0.4f, 0.0001f, 0);	//描画位置
	transform_.scale_		= DirectX::XMVectorSet(2.0f, 1.3f, 0, 0);			//拡大サイズ
}

//更新
void ResultBoardImage::Update()
{
}

//描画
void ResultBoardImage::Draw()
{
	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE_BOARD);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																				//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void ResultBoardImage::Release()
{
}