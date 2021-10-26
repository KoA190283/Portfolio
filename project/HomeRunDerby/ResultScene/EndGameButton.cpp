#include "EndGameButton.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

EndGameButton::EndGameButton() :
	EndGameButton(nullptr)
{
}

EndGameButton::EndGameButton(ResultScene* pResultScene)
{
	pResultScene_ = pResultScene;
}

//初期化
void EndGameButton::Initialize()
{
	//数字の画像データの読み込み
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		//数字iの画像データの読み込み
		const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathNumberImage(i));
		assert(IMAGE_HANDLE != Image::ERROR_CORD);
	}

	//画像の読み込み
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//画像のハンドル番号
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//描画位置の設定
	transform_.position_	= DirectX::XMVectorSet(0.6f, -0.5f, 0, 0);		//描画位置
	transform_.scale_		= DirectX::XMVectorSet(0.9f, 0.9f, 0, 0);		//拡大サイズ
}

//更新
void EndGameButton::Update()
{
	if (pResultScene_ == nullptr) return;	//リザルトシーンのポインタが無ければ更新処理を行わない

	//クリックされているかを確認し、されていなければ更新処理を行わない
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	if (!Image::IsClicked(IMAGE_HANDLE))	return;

	//ゲーム終了
	pResultScene_->EndGame();
}

//描画
void EndGameButton::Draw()
{
	//描画
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//画像へのパス
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//画像のハンドル番号
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//解放
void EndGameButton::Release()
{
}