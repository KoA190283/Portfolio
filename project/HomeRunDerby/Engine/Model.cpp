#include "Model.h"

#include "Camera.h"
#include "Global.h"

//モデルデータの管理
namespace Model 
{
	namespace
	{
		//モデルデータ
		struct ModelData
		{
			std::string fileName;		//ファイル名（パス）
			Transform	transform;		//状態（位置、拡大縮小など）
			Fbx*		pFbx;			//モデルデータ

			//コンストラクタ
			ModelData() :pFbx(nullptr) {}
		};

		std::vector<ModelData*> datas_;	//モデルデータの集合体
	}

	//モデルデータの読み込み
	int Load(const std::string& fileName)
	{
		//既にロード済みであればそのハンドル番号を返す
		const int HANDLE = GetHandle(fileName);
		if (HANDLE != ERROR_CORD)	return HANDLE;

		//FBXデータのロード
		ModelData* pData = new ModelData;
		pData->pFbx		 = new Fbx;
		if (FAILED(pData->pFbx->Load(fileName)))
		{
			SAFE_DELETE(pData->pFbx);
			SAFE_DELETE(pData);
			return ERROR_CORD;
		}

		//ファイル名を設定
		pData->fileName = fileName;

		//モデルデータを格納
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
		//使用するシェーダーの指定がなければDEFAULT_3Dで描画
		Draw(handle, Direct3D::SHADER_TYPE::DEFAULT_3D);
	}

	//描画
	void Draw(int handle, Direct3D::SHADER_TYPE type)
	{
		//使用するシェーダーのセットを指定する
		Direct3D::SetShaderBundle(type);

		//変換行列を再計算
		datas_[handle]->transform.Calclation();

		//描画
		datas_[handle]->pFbx->Draw(datas_[handle]->transform);
	}

	//指定された時間のアニメーションの状態に変形する
	void DeformAnimationPose(int handle, int frame)
	{
		datas_[handle]->pFbx->DeformAnimationPose(frame);
	}

	//指定された時間のアニメーションの姿勢情報を設定する（変形はしない）
	void SetAnimationPose(int handle, int frame)
	{
		datas_[handle]->pFbx->SetAnimationPose(frame);
	}

	//二つのモデルの姿勢の差分を新しい姿勢として設定（変形はしない）
	void SetDifferencePose(int Basehandle, int materialHandle, float percentage)
	{
		datas_[Basehandle]->pFbx->SetDifferencePose(datas_[materialHandle]->pFbx, percentage);
	}

	//二つのモデルの初期姿勢から現在の姿勢への差分を合成し新しい姿勢として設定（変形はしない）
	void AddDifferencePose(int Basehandle, int materialHandle)
	{
		datas_[Basehandle]->pFbx->AddDifferencePose(datas_[materialHandle]->pFbx);
	}

	//設定されている姿勢情報で変形する
	void Deform(int handle)
	{
		datas_[handle]->pFbx->Deform();
	}

	//ボーンの位置を取得する
	XMVECTOR GetBonePosition(int handle, const std::string& boneName)
	{
		return datas_[handle]->pFbx->GetBonePosition(boneName);
	}

	//ロード済みモデルのハンドル番号を取得する
	int GetHandle(const std::string& fileName)
	{
		//ロード済みのモデルデータからファイル名が一致するものを探してハンドル番号を返す
		for (int i = 0; i < datas_.size(); i++)
			if (datas_[i]->fileName == fileName) return i;

		//存在しなければ-1を返す
		return ERROR_CORD;
	}

	//解放
	void Release()
	{
		auto it = datas_.begin();
		while (it != datas_.end())
		{
			SAFE_DELETE((*it)->pFbx);
			SAFE_DELETE((*it));
			it++;
		}
		datas_.clear();
	}

	//レイキャスト
	void RayCast(int handle, RayCastData* rayData)
	{
		using DirectX::operator+;
		using DirectX::operator-;

		//レイのデータをワールド座標からローカル座標に変換
		const XMMATRIX INVERSE_WORLD_MATRIX	= XMMatrixInverse(nullptr, datas_[handle]->transform.GetWorldMatrix());	//ワールド行列の逆行列
		XMVECTOR passingPoint				= rayData->start + rayData->direction;									//レイの通過点
		passingPoint						= XMVector3TransformCoord(passingPoint, INVERSE_WORLD_MATRIX);			//レイ通過点をinvWorldで変換しローカル座標に

		rayData->start		= XMVector3TransformCoord(rayData->start, INVERSE_WORLD_MATRIX);						//レイ始点をinvWorldで変換しローカル座標に
		rayData->direction	= DirectX::XMVector3Normalize(passingPoint - rayData->start);							//始点->通過点のベクトルがレイの向き

		//レイキャスト
		datas_[handle]->pFbx->RayCast(rayData, datas_[handle]->transform);
	}
}