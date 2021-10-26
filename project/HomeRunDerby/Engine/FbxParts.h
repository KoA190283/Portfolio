#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <string>

#include "Fbx.h"
#include "Transform.h"

//FBXSDKを使用するのに必要なライブラリ
#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")


//前方宣言
class  Texture;
struct RayCastData;

const int POLYGON_VERTICES = 3;	//ポリゴンごとの頂点数

//FBXのデータをパーツごとに管理するクラス
class FbxParts
{
	using XMMATRIX		= DirectX::XMMATRIX;
	using XMVECTOR		= DirectX::XMVECTOR;
	using XMFLOAT4		= DirectX::XMFLOAT4;
	using XMFLOAT4X4	= DirectX::XMFLOAT4X4;

private:

	//コンスタントバッファー
	struct CONSTANT_BUFFER
	{
		XMMATRIX vertexConversionMatrix;//頂点変換用行列
		XMMATRIX normalConversionMatrix;//法線変換用行列
		XMMATRIX matWorld;				//ワールド行列

		XMFLOAT4 diffuseColor;			//マテリアルの色
		XMFLOAT4 ambient;				//環境光
		XMFLOAT4 speculer;				//鏡面反射

		XMFLOAT4 cameraPosition;		//カメラの位置
		FLOAT	 shininess;				//輝度
		BOOL	 isTexture;				//テクスチャ貼ってあるかどうか
	};

	//頂点情報
	struct VERTEX
	{
		XMVECTOR position;				//画面内の頂点座標
		XMVECTOR uv;					//UV座標
		XMVECTOR normal;				//法線
		XMVECTOR tangent;				//接線
	};

	//マテリアル
	struct MATERIAL
	{
		XMFLOAT4 diffuse;				//拡散反射光
		XMFLOAT4 ambient;				//環境光
		XMFLOAT4 specular;				//鏡面反射光
		float	 shininess;				//輝度
		Texture* pDiffuseTexture;		//テクスチャ
		Texture* pNormalTexture;		//ノーマルテクスチャ
	};

	//ボーン構造体（関節情報）
	struct  Bone
	{
		XMMATRIX bindPose;				//初期ポーズ時のボーン変換行列
		XMMATRIX newPose;				//アニメーションで変化したときのボーン変換行列
		XMMATRIX diffPose;				//mBindPose に対する mNowPose の変化量
	};

	//ウェイト構造体（ボーンと頂点の関連付け）
	struct Weight
	{
		XMVECTOR positionOrigin;		//元々の頂点座標
		XMVECTOR normalOrigin;			//元々の法線ベクトル
		int*	 pBoneIndex;			//関連するボーンのID
		float*	 pBoneWeight;			//ボーンの重み
	};

	ID3D11Buffer*	pVertexBuffer_;			//頂点バッファ
	ID3D11Buffer**	ppIndexBuffer_;			//インデックスバッファ
	ID3D11Buffer*	pConstantBuffer_;		//コンスタントバッファ

	MATERIAL*		pMaterialDatas_;		//マテリアルのリスト
	VERTEX*			pVertexDatas_;			//頂点情報のリスト
	int**			ppIndexDatas_;			//マテリアル毎のインデックス情報のリスト

	DWORD			vertexCount_;			//頂点数
	DWORD			polygonCount_;			//ポリゴン数
	DWORD			materialCount_;			//マテリアルの個数
	DWORD*			indexCountEachMaterial_;//マテリアル毎のインデックス数

	FbxSkin*		pSkinInfo_;				//スキンメッシュ情報（スキンメッシュアニメーションのデータ本体）
	FbxCluster**	ppCluster_;				//クラスタ情報（関節ごとに関連付けられた頂点情報）
	Bone*			pBoneArray_;			//各関節の情報
	Weight*			pWeightArray_;			//ウェイト情報（頂点の対する各関節の影響度合い）
	int				numBone_;				//FBXに含まれている関節の数

public:

	FbxParts();
	~FbxParts();

	//FBXのデータを使用する準備
	//引数：pNode			FBXファイルのシーン
	//戻値：処理の成否
	HRESULT InitFbxPartsData(FbxNode* pNode);

	//描画
	//引数：transform		変換情報
	void Draw(Transform & transform) const;

	//アニメーションの姿勢に変化させるための行列を取得
	//引数１：BoneNumber	取得するボーンの番号
	//引数２：time			アニメーションのフレーム
	FbxMatrix GetAnimationMatrix(int BoneNumber, const FbxTime& time);

	//アニメーションに合わせて姿勢を設定
	//引数：time			アニメーションのフレーム
	void SetAnimationPose(const FbxTime& time);

	//二つの姿勢の差分を新しい姿勢として設定
	//引数１：pFbxParts		差分を取るFbxParts
	//引数２：percentage	pFbxPartsの姿勢にどれだけ寄せるかの比率
	void SetDifferencePose(FbxParts* pFbxParts, float percentage);

	//二つのFBXの初期姿勢から現在の姿勢への差分を合成する
	//引数１：pFbxParts		合成するFbxParts
	void AddDifferencePose(FbxParts * pFbxParts);

	//変形する
	void Deform();

	//指定した名前のボーンの位置を取得
	//引数１：boneName　	位置を取得するボーンの名前
	//引数２：position		取得した位置を格納するアドレス
	//戻値　：引数１で指定した名前のボーンが存在したか
	bool GetBonePosition(const std::string& boneName, XMVECTOR* position) const;

	//レイキャスト
	//引数１：pData　		レイキャストに使用するデータ
	//引数２：transform		変換情報
	void RayCast(RayCastData* pData, Transform& transform) const;

	//解放
	void Release();

private:

	//頂点バッファ準備
	//引数：pMesh		メッシュ情報
	//戻値：処理の成否
	HRESULT InitVertex(FbxMesh* pMesh);

	//頂点情報をクラス変数に格納
	//引数：pMesh		メッシュ情報
	void SetVertexData(FbxMesh* pMesh);

	//頂点バッファの作成
	//戻値：処理の成否
	HRESULT CreateVertexBuffer();

	//インデックスバッファ準備
	//引数：pMesh		メッシュ情報
	//戻値：処理の成否
	HRESULT InitIndex(FbxMesh* pMesh);

	//インデックス情報をクラス変数に格納
	//引数１：pMesh		メッシュ情報
	//引数２：material	取得するマテリアルの要素番号
	void SetIndexData(FbxMesh* pMesh,int material);

	//インデックスバッファの作成
	//引数１：material		取得するマテリアルの要素番号
	//引数２：indexCount	取得するマテリアルのインデックス数
	//戻値　：処理の成否
	HRESULT CreateIndexBuffer(int material, int indexCount);

	//コンスタントバッファ準備
	//戻値：処理の成否
	HRESULT InitConstantBuffer();

	//マテリアル準備
	//引数：pNode	ノード情報
	//戻値：処理の成否
	HRESULT InitMaterial(FbxNode* pNode);

	//テクスチャの読み込み
	//引数１：pMaterial	マテリアルデータ
	//引数２：material	取得するマテリアルの要素番号
	//戻値　：処理の成否
	HRESULT InitDiffuseTexture(fbxsdk::FbxSurfaceMaterial * pMaterial, int material);

	//法線マップテクスチャの読み込み
	//引数１：pMaterial	マテリアルデータ
	//引数２：material	取得するマテリアルの要素番号
	//戻値　：処理の成否
	HRESULT InitNormalTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material);

	
	//骨の情報を準備
	//引数：pMesh		メッシュ情報
	void InitSkelton(FbxMesh* pMesh);

	//ボーンのウエイトの作成
	void CreateBoneWeight();

	//ボーンの影響力を頂点ごとに整理して格納
	void SetBoneWeight();

	//ボーンの作成
	void CreateBone();

	//マテリアルごとに描画
	//引数：transform			描画時の変形情報
	void DrawMaterial(Transform& transform) const;
};