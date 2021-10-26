#include "Fbx.h"

#include "Direct3D.h"
#include "Camera.h"
#include "Math.h"
#include "Global.h"

using DirectX::XMVECTOR;

//初期化
Fbx::Fbx() :
	pFbxManager_(nullptr),
	pFbxScene_(nullptr),
	frameRate_(FbxTime::EMode::eDefaultMode)
{
};

//デストラクタ
Fbx::~Fbx()
{
	Release();
};

//FBXデータのロード
HRESULT Fbx::Load(std::string const & fileName)
{
	//FbxSceneにデータを入れる
	if (FAILED(InitFbxScene(fileName)))
	{
		return E_FAIL;
	}

	//FBXのデータをクラスに格納
	if (FAILED(InitFbxParts(fileName)))
	{
		return E_FAIL;
	}
	
	return S_OK;
};

//描画
void Fbx::Draw(Transform & transform) const
{	
	//パーツごとに描画
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->Draw(transform);
	}
};

//アニメーションに合わせて変形する
void Fbx::DeformAnimationPose(int frame)
{
	//アニメーションに合わせてボーンを設定
	SetAnimationPose(frame);

	//変形する
	Deform();
}

//アニメーションに合わせてボーンを設定
void Fbx::SetAnimationPose(int frame)
{
	//FbxTimeをframeに設定する
	FbxTime fbxTime;
	fbxTime.SetTime(0, 0, 0, frame, 0, 0, frameRate_);

	//パーツごとに設定
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->SetAnimationPose(fbxTime);
	}
}

//二つのFBXの姿勢の差分を新しい姿勢として設定
void Fbx::SetDifferencePose(Fbx * pFbx, float percentage)
{
	if (pFbx == nullptr)	return;				//引数ポインタがnullptrであれば処理不可

	//パーツごとに設定
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->SetDifferencePose(pFbx->fbxPartsList_[i], percentage);
	}
}

//二つのFBXの初期姿勢から現在の姿勢への差分を合成する
void Fbx::AddDifferencePose(Fbx * pFbx)
{
	if (pFbx == nullptr)	return;				//引数ポインタがnullptrであれば処理不可

	//パーツごとに設定
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->AddDifferencePose(pFbx->fbxPartsList_[i]);
	}
}

//変形する
void Fbx::Deform()
{
	//パーツごとに設定
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->Deform();
	}
}

//ボーンの位置を取得
XMVECTOR Fbx::GetBonePosition(const std::string& boneName) const
{
	XMVECTOR bonePosision = DirectX::g_XMZero;

	//パーツごとに名前の一致するボーンを探し、あったらの位置を
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		//ボーンの位置が取得出来たらループを抜ける
		if (fbxPartsList_[i]->GetBonePosition(boneName, &bonePosision))	break;
	}

	return bonePosision;
}

//レイキャスト
void Fbx::RayCast(RayCastData* pData, Transform& transform) const
{
	if (pData == nullptr)	return;				//引数ポインタがnullptrであれば処理不可

	pData->isHit = false;

	//パーツごとにレイキャスト
	for (int i = 0; i < fbxPartsList_.size(); i++)
	{
		fbxPartsList_[i]->RayCast(pData, transform);

		//衝突していたら終了
		if (pData->isHit)	return;
	}
}

//解放
void Fbx::Release()
{
	pFbxScene_->Destroy();
	pFbxManager_->Destroy();
};

//FbxSceneにデータを入れる
HRESULT Fbx::InitFbxScene(const std::string& fileName)
{
	FbxImporter* pFbxImporter = nullptr;	//Fbxファイルの持つデータをメッシュやカメラなどの情報に分解するやつ

	//マネージャを生成
	if ((pFbxManager_ = FbxManager::Create()) == nullptr)
	{
		MessageBox(nullptr, "FBXマネージャーの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//インポーターを生成
	if ((pFbxImporter = FbxImporter::Create(pFbxManager_, "imp")) == nullptr)
	{
		MessageBox(nullptr, "FBXインポーターの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//指定したFBXファイルで初期化する
	if (pFbxImporter->Initialize(fileName.c_str(), -1, pFbxManager_->GetIOSettings()) == false)
	{
		MessageBox(nullptr, "ファイルの初期化に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//FBXシーンの作成
	if ((pFbxScene_ = FbxScene::Create(pFbxManager_, "fbxscene")) == nullptr)
	{
		MessageBox(nullptr, "FBXシーンの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	//シーンオブジェクトにFBXファイルの情報を流し込む
	if (pFbxImporter->Import(pFbxScene_) == false)
	{
		MessageBox(nullptr, "データのインポートに失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	pFbxImporter->Destroy();

	return S_OK;
}

//FBXのデータを使用する準備
HRESULT Fbx::InitFbxParts(const std::string& fileName)
{
	//現在のカレントディレクトリを覚えておく
	char defaultCurrentDirectory[MAX_PATH];				//カレントディレクトリのパス
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	char fileDirectory[_MAX_PATH];						//FBXファイルがあるディレクトリ（フォルダ）名

	//パスの取得
	_splitpath_s(fileName.c_str(), nullptr, 0, fileDirectory, _MAX_PATH, nullptr, 0, nullptr, 0);

	//カレントディレクトリ変更
	SetCurrentDirectory(fileDirectory);

	//ルートノードの取得
	FbxNode* rootNode = pFbxScene_->GetRootNode();

	//子の数だけノードを調べる
	const int CHILD_COUNT = rootNode->GetChildCount();
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		CheckNode(rootNode->GetChild(i));
	}

	//フレームレートの取得
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	//カレントディレクトリを元に戻す
	SetCurrentDirectory(defaultCurrentDirectory);

	return S_OK;
}

//ノードの中にメッシュ情報があるかを確認し、あればパーツを使用可能な状態にする
void Fbx::CheckNode(FbxNode * pNode)
{
	//そのノードにはメッシュ情報が入っているだろうか？
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//パーツを用意
		FbxParts* pParts = new FbxParts;
		pParts->InitFbxPartsData(pNode);

		//パーツ情報を動的配列に追加
		fbxPartsList_.push_back(pParts);
	}

	//子ノードにもデータを確認
	int childCount = pNode->GetChildCount();

	//一人ずつチェック
	for (int i = 0; i < childCount; i++)
	{
		CheckNode(pNode->GetChild(i));
	}
}