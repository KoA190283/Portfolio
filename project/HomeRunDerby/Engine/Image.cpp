#include "Image.h"

#include<string>
#include<vector>

#include"Input.h"
#include"Global.h"

namespace Image
{
	namespace
	{
		//画像データ
		struct ImageData
		{
			std::string		fileName;	//ファイル名（パス）
			Transform		transform;	//状態（位置、拡大縮小など）
			Sprite*			pSprite;	//画像データ

			//コンストラクタ
			ImageData() :pSprite(nullptr)
			{}
		};

		std::vector<ImageData*> datas_;	//画像データの集合体
	}

	//画像データの読み込み
	int Load(const std::string& fileName)
	{
		//ロード済みデータだったらそのデータのハンドルを返す
		const int HANDLE = GetHandle(fileName);
		if (HANDLE != ERROR_CORD)	return HANDLE;
		
		//画像データのロード
		ImageData* pData = new ImageData;
		pData->pSprite	 = new Sprite;
		if (FAILED(pData->pSprite->Initialize(fileName)))
		{
			SAFE_DELETE(pData->pSprite);
			SAFE_DELETE(pData);
			return ERROR_CORD;
		}

		//ファイル名を設定
		pData->fileName = fileName;

		//画像データを格納
		datas_.push_back(pData);
		
		//ハンドルを返す
		return (int)datas_.size() - 1;
	}
	
	//表示する状態を設定
	void SetTransform(int handle, Transform& transform)
	{
		datas_[handle]->transform = transform;
	}
	
	//描画
	void Draw(int handle)
	{
		//使用するシェーダーの指定がなければDEFAULT_2Dで描画
		Draw(handle, Direct3D::SHADER_TYPE::DEFAULT_2D);
	}

	//描画
	void Draw(int handle, Direct3D::SHADER_TYPE type)
	{
		//使用するシェーダーのセットを指定する
		Direct3D::SetShaderBundle(type);

		//変換行列を再計算
		datas_[handle]->transform.Calclation();

		//描画
		datas_[handle]->pSprite->Draw(datas_[handle]->transform);
	}
	
	//自然数の描画
	void DrawNaturalNumber(int handle[DECIMAL_BASE], int drawNumber, Transform const& lastDigitTransform, float intervalWide)
	{
		const std::string DRAW_NUMBERS = std::to_string(drawNumber);			//描画する数字を文字列に変換したもの

		//描画する画像の変形状態を決める
		Transform transform			=  lastDigitTransform;						//描画する変形情報
		transform.position_.vecX	-= DRAW_NUMBERS.size() * intervalWide;		//positionが最下位桁の描画位置になるように描画位置をずらす
		transform.scale_			= lastDigitTransform.scale_;				//数字の拡大サイズの設定

		//数字列を最上位桁から一桁ずつ描画する
		for (auto number : DRAW_NUMBERS) 
		{
			//描画
			int DRAW_NUMBER = number - '0';										//描画する数字
			Image::SetTransform(handle[DRAW_NUMBER], transform);
			Image::Draw(handle[DRAW_NUMBER]);

			//1桁分位置をずらす
			transform.position_.vecX += intervalWide;
		}
	}

	//ロード済み画像のハンドル番号を取得する
	int GetHandle(std::string const& fileName)
	{
		//ロード済みの画像データからファイル名が一致するものを探してハンドル番号を返す
		for (int i = 0; i < datas_.size(); i++)
			if (datas_[i]->fileName == fileName) return i;

		//存在しなければ-1を返す
		return ERROR_CORD;
	}

	//幅を取得
	float GetWidth(int handle)
	{
		return datas_[handle]->pSprite->GetWidth() * datas_[handle]->transform.scale_.vecX;
	}

	//高さを取得
	float GetHeight(int handle)
	{
		return datas_[handle]->pSprite->GetHeight() * datas_[handle]->transform.scale_.vecY;
	}

	//画像がクリックされているか
	bool IsClicked(int handle)
	{
		if (!Input::IsMouseButtonDown(0))	return false;									//右クリックされていなければ何もしない

		const float HALF = 0.5f;															//半分	

		//マウスの位置を設定
		XMVECTOR mousePosition	=  Input::GetMousePosition();								//マウスの位置
		mousePosition.vecX		-= Global::WINDOW_WIDTH * HALF;								//X座標をウィンドウの幅で調整
		mousePosition.vecY		-= Global::WINDOW_HEIGHT * HALF;							//Y座標をウィンドウの高さで調整
		mousePosition.vecY		*= -1.0f;													//Y座標を反転する

		//画像の情報を取得
		const float	HALF_IMAGE_WIDTH	= GetWidth(handle) * HALF;							//描画画像の半分の幅
		const float HALF_IMAGE_HEIGHT	= GetHeight(handle) * HALF;							//描画画像の半分の高さ

		//画像の位置を設定
		XMVECTOR ImagePosition	=  datas_[handle]->transform.position_;						//画像の位置															
		ImagePosition.vecX		*= Global::WINDOW_WIDTH * HALF;								//X座標をウィンドウの幅で調整	
		ImagePosition.vecY		*= Global::WINDOW_HEIGHT * HALF;							//Y座標をウィンドウの高さで調整

		//画像の外側にマウスがあるかを確認し外ならfalseを返す
		if (mousePosition.vecY > ImagePosition.vecY + HALF_IMAGE_HEIGHT)	return false;	//上
		if (mousePosition.vecY < ImagePosition.vecY - HALF_IMAGE_HEIGHT)	return false;	//下
		if (mousePosition.vecX < ImagePosition.vecX - HALF_IMAGE_WIDTH)		return false;	//左
		if (mousePosition.vecX > ImagePosition.vecX + HALF_IMAGE_WIDTH)		return false;	//右

		//クリックされている
		return true;
	}

	//解放
	void Release()
	{
		auto it = datas_.begin();
		while (it != datas_.end())
		{
			SAFE_DELETE((*it)->pSprite);
			SAFE_DELETE((*it));
			it++;
		}
		datas_.clear();
	}
}