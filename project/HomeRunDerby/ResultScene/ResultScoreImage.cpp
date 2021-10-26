#include "ResultScoreImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"

ResultScoreImage::ResultScoreImage()
{
}

//初期化
void ResultScoreImage::Initialize()
{
	//数字の画像データの読み込み
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		//数字iの画像データの読み込み
		const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathNumberImage(i));
		assert(IMAGE_HANDLE != Image::ERROR_CORD);
	}

	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(-0.17f, 0.5f, 0, 0);		//描画位置
	transform_.scale_		= DirectX::XMVectorSet(1.3f, 1.3f, 0, 0);		//拡大サイズ
}

//更新
void ResultScoreImage::Update()
{
}

//描画
void ResultScoreImage::Draw()
{
	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);

	//数字描画用画像のハンドル番号配列作成
	int numberImageHandles[ExternalDataManager::DECIMAL_BASE];													//数字描画用画像のハンドル番号配列
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		numberImageHandles[i] = Image::GetHandle(ExternalDataManager::GetFilePathNumberImage(i));
	}

	//描画位置を文字列分ずらす
	const float SCORE_IMAGE_WIDE = 0.7f;
	transform_.position_.vecX	+= SCORE_IMAGE_WIDE;

	//得点数の描画
	const float INTERVAL_WIDE = 0.07f;																			//1文字ごとの間隔
	Image::DrawNaturalNumber(numberImageHandles, InternalPlayDataReservoir::GetScore(), transform_, INTERVAL_WIDE);

	//描画位置を戻す
	transform_.position_.vecX -= SCORE_IMAGE_WIDE;
}

//解放
void ResultScoreImage::Release()
{
}