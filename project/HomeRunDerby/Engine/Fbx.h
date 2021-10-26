#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include <vector>

#include "FbxParts.h"
#include "Transform.h"

//FBXSDKを使用するのに必要なライブラリ
#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")

//レイキャスト用構造体
struct RayCastData
{
	using XMVECTOR = DirectX::XMVECTOR;

	XMVECTOR	start;			//レイ発射位置
	XMVECTOR	direction;		//レイの向きベクトル
	float		distance;		//衝突点までの距離
	bool        isHit;			//レイが当たったか
};

//前方宣言
class FbxParts;

//FBXを管理するクラス
class Fbx
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	FbxManager*				pFbxManager_;			//FBXファイルを扱う機能の本体
	FbxScene*				pFbxScene_;				//FBXファイルのシーン

	std::vector<FbxParts*>	fbxPartsList_;			//FBXのパーツごとのデータ
	FbxTime::EMode			frameRate_;				//アニメーションのフレームレート

public:

	Fbx();
	~Fbx();

	//FBXデータのロード
	//引数：fileName		ロードするFBXファイルの名前
	//戻値：処理の成否
	HRESULT Load(std::string const & fileName);

	//描画
	//引数：transform		変換情報
	void Draw(Transform & transform) const;

	//アニメーションに合わせて変形する
	//引数：frame			アニメーションのフレーム
	void DeformAnimationPose(int frame);
	
	//アニメーションに合わせて姿勢を設定（変形はしない）
	//引数：frame			アニメーションのフレーム
	void SetAnimationPose(int frame);

	//二つのFBXの姿勢の差分を新しい姿勢として設定（変形はしない）
	//引数１：pFbx			アニメーションを合成するFBX
	//引数２：percentage	合成する比率
	void SetDifferencePose(Fbx* pFbx, float percentage);

	//二つのFBXの初期姿勢から現在の姿勢への差分を合成する
	//引数：pFbx			アニメーションを合成するFBX
	void AddDifferencePose(Fbx* pFbx);

	//変形する
	void Deform();

	//ボーンの位置を取得
	//引数：boneName		位置を取得するボーンの名前
	//戻値：ボーンの位置
	XMVECTOR GetBonePosition(const std::string& boneName) const;

	//レイキャスト
	//引数：data　			レイキャストに使用するデータ
	//引数：transform		変換情報
	void RayCast(RayCastData * pData, Transform& transform) const;

	//解放
	void Release();

private:

	//FbxSceneにデータを入れる
	//引数：fileName		ロードするFBXファイルの名前
	//戻値：処理の成否
	HRESULT InitFbxScene(const std::string& fileName);

	//FBXのデータを使用する準備
	//引数：fileName		ロードするFBXファイルの名前
	//戻値：処理の成否
	HRESULT InitFbxParts(const std::string& fileName);

	//ノードの中にメッシュ情報があるかを確認し、あればパーツを使用可能な状態にする
	//引数：pNode	ノード情報
	void CheckNode(FbxNode* pNode);
};