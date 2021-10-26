#include "BackGround.h"

#include "Engine/Image.h"
#include "ExternalDataManager.h"

//背景描画
namespace BackGround
{
	//変数
	namespace
	{
		BACK_GROUND_TYPE currentBackGroundType_;		//現在設定されている背景
	}

	//初期化
	void Initialize()
	{
		//背景描画に使用するデータを読み込み
		ExternalDataManager::LoadBackGroundData("Data/BackGroundData.csv");

		//モデルデータの読み込み
		for (int i = 0; i < (int)BACK_GROUND_TYPE::MAX; i++) 
		{
			const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathBackGroundModel(i));
			assert(IMAGE_HANDLE != Image::ERROR_CORD);
		}

		//空背景を初期設定にする
		currentBackGroundType_ = BACK_GROUND_TYPE::SKY;
	}

	//描画
	void Draw()
	{
		using DirectX::operator*=;

		//描画するモデルの状態を設定して描画
		const int					IMAGE_HANDLE	= Image::GetHandle(ExternalDataManager::GetFilePathBackGroundModel((int)currentBackGroundType_));	//モデルハンドル
		const Direct3D::SHADER_TYPE SHADER_TYPE		= (Direct3D::SHADER_TYPE)ExternalDataManager::GetShaderTypeBackGround((int)currentBackGroundType_);	//使用するシェーダー

		Transform transform;
		transform.position_.vecZ	+= 0.9999999f;		//Zは深度値扱い(0~1) 1を超えると描画されない
		Image::SetTransform(IMAGE_HANDLE, transform);
		Image::Draw(IMAGE_HANDLE, SHADER_TYPE);
	}

	//使用する背景を変更する
	void ChangeBackGround(BACK_GROUND_TYPE type)
	{
		currentBackGroundType_ = type;
	}

	//解放
	void Release()
	{
	}
}

