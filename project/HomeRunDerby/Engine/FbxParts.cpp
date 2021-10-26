#include "FbxParts.h"

#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"
#include "Math.h"
#include "Global.h"

//初期化
FbxParts::FbxParts() :

	pVertexBuffer_(nullptr),
	ppIndexBuffer_(nullptr),
	pConstantBuffer_(nullptr),

	pMaterialDatas_(nullptr),
	pVertexDatas_(nullptr),
	ppIndexDatas_(nullptr),

	vertexCount_(0),
	polygonCount_(0),
	materialCount_(0),
	indexCountEachMaterial_(nullptr),

	pSkinInfo_(nullptr),
	ppCluster_(nullptr),
	pBoneArray_(nullptr),
	pWeightArray_(nullptr),
	numBone_(0)
{
}; 

//デストラクタ
FbxParts::~FbxParts()
{
	Release();
};

//FBXのデータを使用する準備
HRESULT FbxParts::InitFbxPartsData(FbxNode* pNode)
{
	if (pNode == nullptr)	return E_FAIL;					//引数ポインタがnullptrであれば処理不可

	//メッシュ情報を取得
	FbxMesh* pMesh		= pNode->GetMesh();					//メッシュ情報

	//各情報の個数を取得
	vertexCount_		= pMesh->GetControlPointsCount();	//頂点の数
	polygonCount_		= pMesh->GetPolygonCount();			//ポリゴンの数
	materialCount_		= pNode->GetMaterialCount();		//マテリアル数

	//頂点バッファ準備
	if (FAILED(InitVertex(pMesh))) return E_FAIL;

	//インデックスバッファ準備
	if (FAILED(InitIndex(pMesh))) return E_FAIL;

	//コンスタントバッファ準備
	if (FAILED(InitConstantBuffer())) return E_FAIL;

	//マテリアル準備
	if (FAILED(InitMaterial(pNode))) return E_FAIL;

	//スケルトンの準備
	InitSkelton(pMesh);

	return S_OK;
}

//描画
void FbxParts::Draw(Transform & transform) const
{
	//頂点バッファをセット
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::GetContext()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//コンスタントバッファをセット
	Direct3D::GetContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用
	Direct3D::GetContext()->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

	//描画
	DrawMaterial(transform);
};

//アニメーションの姿勢に変化させるための行列を取得
FbxMatrix FbxParts::GetAnimationMatrix(int const BoneNumber, const FbxTime& time)
{
	//このFBXのBoneNumber目のボーンがtimeの時の情報を取得する
	FbxAnimEvaluator* pEvaluator = ppCluster_[BoneNumber]->GetLink()->GetScene()->GetAnimationEvaluator();
	return pEvaluator->GetNodeGlobalTransform(ppCluster_[BoneNumber]->GetLink(), time);
}

//アニメーションに合わせて姿勢を設定
void FbxParts::SetAnimationPose(const FbxTime& time)
{
	//ボーンごとの現在の行列を取得する
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//timeの時のボーンの変形行列を取得する
		FbxMatrix currentPose = GetAnimationMatrix(bone_i, time);

		// 行列コピー（Fbx形式からDirectXへの変換）
		XMFLOAT4X4 pose;
		for (DWORD y = 0; y < 4; y++) {
			for (DWORD x = 0; x < 4; x++)
			{
				pose(x, y) = (float)currentPose.Get(x, y);
			}

			//右手系(Maya)から左手系(DirectX)へ変換
			pose.m[y][0] *= -1.f;
		}

		// オフセット時のポーズの差分を計算してボーンのデータを変更する
		pBoneArray_[bone_i].newPose	 =  XMLoadFloat4x4(&pose);
		pBoneArray_[bone_i].diffPose =  XMMatrixInverse(nullptr, pBoneArray_[bone_i].bindPose);
		pBoneArray_[bone_i].diffPose *= pBoneArray_[bone_i].newPose;
	}
}

//二つの姿勢の差分を新しい姿勢として設定
void FbxParts::SetDifferencePose(FbxParts * pFbxParts, float percentage)
{
	if (pFbxParts == nullptr)	return;															//引数ポインタがnullptrであれば処理不可

	//ボーンごとに二つの姿勢の差分を新しい姿勢として設定
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//二つの姿勢の差分を取得しpercentageを掛けて差分の大きさを調整する
		const XMMATRIX DIFF_COMPOSITION_POSE = (pBoneArray_[bone_i].diffPose - pFbxParts->pBoneArray_[bone_i].diffPose) * percentage;	//現在の姿勢からpFbxPartsの姿勢へのpercentage分の差

		//オフセット時のポーズからの差分をDIFF_COMPOSITION_POSEとしてボーンのデータを変更する
		pBoneArray_[bone_i].newPose  = pBoneArray_[bone_i].bindPose + DIFF_COMPOSITION_POSE;	//新しい姿勢;	
		pBoneArray_[bone_i].diffPose = DIFF_COMPOSITION_POSE;									//初期姿勢との差分
	}
}

//現在の姿勢にpFbxPartsのdiffPoseを加算合成する
void FbxParts::AddDifferencePose(FbxParts * pFbxParts)
{
	if (pFbxParts == nullptr)	return;												//引数ポインタがnullptrであれば処理不可

	//ボーンごとに現在の姿勢をpercentage分だけpFbxPartsの姿勢に近づける
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//オフセット時のポーズの差分を計算してボーンのデータを変更する
		pBoneArray_[bone_i].diffPose += pFbxParts->pBoneArray_[bone_i].diffPose;	//初期姿勢との差分
		pBoneArray_[bone_i].newPose	 += pFbxParts->pBoneArray_[bone_i].diffPose;	//新しい姿勢;	
	}
}

//変形する
void FbxParts::Deform()
{
	//各ボーンに対応した頂点の変形制御
	for (DWORD vertex_i = 0; vertex_i < vertexCount_; vertex_i++)
	{
		//各頂点ごとに影響するボーンのウェイト値を反映させた関節行列を作成する
		XMMATRIX  matrix;
		ZeroMemory(&matrix, sizeof(matrix));
		for (int bone_i = 0; bone_i < numBone_; bone_i++)
		{
			if (pWeightArray_[vertex_i].pBoneIndex[bone_i] < 0)	break;																	//頂点にウェイトをもつボーンがなくなったら行列作成ループを抜ける															

			matrix += pBoneArray_[pWeightArray_[vertex_i].pBoneIndex[bone_i]].diffPose * pWeightArray_[vertex_i].pBoneWeight[bone_i];	//関節行列にボーンのウェイトを反映させる
		}

		//作成された関節行列を使って、頂点を変形する
		const XMVECTOR POSITION				= pWeightArray_[vertex_i].positionOrigin;		//変形前の頂点座標
		const XMVECTOR NORMAL				= pWeightArray_[vertex_i].normalOrigin;			//変形前の法線
		pVertexDatas_[vertex_i].position	= XMVector3TransformCoord(POSITION, matrix);	//頂点位置を関節行列で変換
		pVertexDatas_[vertex_i].normal		= XMVector3TransformCoord(NORMAL, matrix);		//法線を関節行列で変換
	}

	//頂点バッファをロックして、変形させた後の頂点情報で上書きする
	D3D11_MAPPED_SUBRESOURCE msr		= {};						//サブリソースデータへのアクセス
	ID3D11DeviceContext*	 pContext	= Direct3D::GetContext();	//デバイスコンテキストのポインタ

	pContext->Map(pVertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy_s(msr.pData, msr.RowPitch, pVertexDatas_, sizeof(VERTEX) * vertexCount_);
	pContext->Unmap(pVertexBuffer_, 0);
}

//ボーンの位置を取得
bool FbxParts::GetBonePosition(const std::string& boneName, XMVECTOR * position) const
{
	if (position == nullptr)	return false;				//引数ポインタがnullptrであれば処理を行わない

	//名前がboneNameのボーンを探して位置を取得する
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//名前が一致したら現在の姿勢でのボーンの位置の位置を取得する
		if (boneName == ppCluster_[bone_i]->GetLink()->GetName())
		{
			//ボーンの現在の姿勢での位置を取得する
			*position = XMVector3TransformCoord(DirectX::g_XMZero, pBoneArray_[bone_i].newPose);
			
			return true;
		}
	}

	return false;
}

//レイキャスト
void FbxParts::RayCast(RayCastData* pData, Transform & transform) const
{
	if (pData == nullptr)	return;				//引数ポインタがnullptrであれば処理不可

	//マテリアル分ループ
	for (DWORD material_i = 0; material_i < materialCount_; material_i++)
	{
		//マテリアルのポリゴン数分ループ
		for (DWORD polygon_i = 0; polygon_i < indexCountEachMaterial_[material_i] / POLYGON_VERTICES; polygon_i++)
		{
			//判定するポリゴンの頂点座標を設定
			XMVECTOR vertex[3];
			vertex[0] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 0]].position;	//１つ目の頂点の座標
			vertex[1] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 1]].position;	//２つ目の頂点の座標
			vertex[2] = pVertexDatas_[ppIndexDatas_[material_i][polygon_i * POLYGON_VERTICES + 2]].position;	//３つ目の頂点の座標

			//レイと三角ポリゴンが交差しているか判定
			pData->isHit = Math::IntersectLineToPolygon(pData->start, pData->direction, vertex[0], vertex[1], vertex[2], &pData->distance);

			//当たってたら終わり
			if (pData->isHit) return;
		}
	}
}

//解放
void FbxParts::Release()
{
	if (pWeightArray_ != nullptr)
	{
		for (DWORD i = 0; i < vertexCount_; i++)
		{
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneIndex);
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneWeight);
		}
		SAFE_DELETE_ARRAY(pWeightArray_);
	}

	SAFE_DELETE_ARRAY(pBoneArray_);
	SAFE_DELETE_ARRAY(ppCluster_);

	SAFE_DELETE_ARRAY(indexCountEachMaterial_);

	for (DWORD i = 0; i < materialCount_; i++)
	{
		SAFE_DELETE_ARRAY(ppIndexDatas_[i]);
		SAFE_RELEASE(pMaterialDatas_[i].pDiffuseTexture);
		SAFE_RELEASE(pMaterialDatas_[i].pNormalTexture);
		SAFE_RELEASE(ppIndexBuffer_[i]);
	}

	SAFE_DELETE_ARRAY(ppIndexDatas_);
	SAFE_DELETE_ARRAY(pVertexDatas_);
	SAFE_DELETE_ARRAY(pMaterialDatas_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_DELETE_ARRAY(ppIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
};

//頂点バッファ準備
HRESULT FbxParts::InitVertex(FbxMesh* pMesh)
{
	//頂点情報をクラス変数に格納
	SetVertexData(pMesh);

	//頂点バッファの作成
	if (FAILED(CreateVertexBuffer())) return E_FAIL;

	return S_OK;
}

//頂点情報をクラス変数に格納
void FbxParts::SetVertexData(FbxMesh* pMesh)
{
	pVertexDatas_ = new VERTEX[vertexCount_];	//頂点情報を入れる配列

	//全ポリゴン
	for (DWORD polygon_i = 0; polygon_i < polygonCount_; polygon_i++)
	{
		//接線・法線情報が存在するかを確認
		int							startIndex		= pMesh->GetPolygonVertexIndex(polygon_i);			//インデックスの開始位置を取得
		FbxGeometryElementTangent*	pElementTangent = pMesh->GetElementTangent(0);						//接線要素を取得
		bool						tangentExists	= pElementTangent != nullptr;						//接線情報が存在するか
		bool						normalExists	= pMesh->GetLayer(0)->GetNormals();					//法線情報が存在するか

		//接線情報を取得
		FbxVector4					tangent;															//接線要素
		if(tangentExists) tangent = pElementTangent->GetDirectArray().GetAt(startIndex).mData;			//接線情報が存在したら接線要素から接線情報の取得

		//3頂点分
		for (int vertex_i = 0; vertex_i < POLYGON_VERTICES; vertex_i++)
		{
			//調べる頂点の番号
			int index = pMesh->GetPolygonVertex(polygon_i, vertex_i);

			//頂点の位置
			FbxVector4 vertexPosition = pMesh->GetControlPointAt(index);																				//メッシュ情報から頂点座標を取得
			pVertexDatas_[index].position = DirectX::XMVectorSet((float)-vertexPosition[0], (float)vertexPosition[1], (float)vertexPosition[2], 0.0f);	//座標を格納

			//頂点のUV
			FbxLayerElementUV* pUv = pMesh->GetLayer(0)->GetUVs();																						//メッシュ情報からuv情報を取得
			int					uvIndex = pMesh->GetTextureUVIndex(polygon_i, vertex_i, FbxLayerElement::eTextureDiffuse);								//メッシュ情報からuvインデックス情報を取得
			FbxVector2			uv = pUv->GetDirectArray().GetAt(uvIndex);																				//uv値を取得
			pVertexDatas_[index].uv = DirectX::XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);								//UV値を格納

			//頂点の法線
			if (normalExists)
			{
				FbxVector4 Normal;																														//法線情報
				pMesh->GetPolygonVertexNormal(polygon_i, vertex_i, Normal);																				//メッシュ情報から法線情報を取得
				pVertexDatas_[index].normal = DirectX::XMVectorSet((float)-Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);						//法線を格納
			}
			else
			{
				pVertexDatas_[index].normal = DirectX::g_XMZero;																						//法線情報が無ければ0ベクトルを格納
			}

			//頂点の接線
			if (tangentExists)
			{
				pVertexDatas_[index].tangent = DirectX::XMVectorSet((float)tangent[0], (float)tangent[1], (float)tangent[2], 0.0f);						//接線を格納
			}
			else 
			{
				pVertexDatas_[index].tangent = DirectX::g_XMZero;																						//接線情報が無ければ0ベクトルを格納
			}
		}
	}
}

//頂点バッファの作成
HRESULT FbxParts::CreateVertexBuffer()
{
	//頂点データ用バッファの設定
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth				= sizeof(VERTEX) * vertexCount_;	//バッファーのサイズ
	bd_vertex.Usage					= D3D11_USAGE_DYNAMIC;				//バッファーの読み取りおよび書き込みが予想される方法
	bd_vertex.BindFlags				= D3D11_BIND_VERTEX_BUFFER;			//バッファーがパイプラインにバインドされる方法
	bd_vertex.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;			//CPUアクセスフラグ
	bd_vertex.MiscFlags				= 0;								//その他のフラグ
	bd_vertex.StructureByteStride	= 0;								//バッファー構造内の各要素のサイズ

	//初期化データの設定
	D3D11_SUBRESOURCE_DATA data_vertex;									//初期化データ
	data_vertex.pSysMem = pVertexDatas_;								//初期化データへのポインタ

	//頂点バッファの作成
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_)))
	{
		MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//インデックスバッファ準備
HRESULT FbxParts::InitIndex(FbxMesh* pMesh)
{
	ppIndexBuffer_			= new ID3D11Buffer*[materialCount_];	//マテリアルの個数分メモリ確保
	indexCountEachMaterial_ = new DWORD[materialCount_];			//マテリアルの個数分メモリ確保
	ppIndexDatas_			= new int*[materialCount_];				//マテリアルの個数分メモリ確保

	//マテリアル数分ループ
	for (DWORD i = 0; i < materialCount_; i++)
	{
		//インデックス情報をクラス変数に格納
		SetIndexData(pMesh, i);

		//インデックスバッファの作成
		if (FAILED(CreateIndexBuffer(i, indexCountEachMaterial_[i]))) return E_FAIL;
	}

	return S_OK;
}

//インデックス情報をクラス変数に格納
void FbxParts::SetIndexData(FbxMesh * pMesh, int material)
{
	ppIndexDatas_[material] = new int[polygonCount_ * POLYGON_VERTICES];			//ポリゴン数 * ポリゴンの頂点数
	int indexCount = 0;																//インデックスの個数

	//全ポリゴン
	for (DWORD polygon_i = 0; polygon_i < polygonCount_; polygon_i++)
	{
		//要素へのインデックス番号を取得
		FbxLayerElementMaterial *pMaterial = pMesh->GetLayer(0)->GetMaterials();	//マテリアル
		assert(pMaterial != nullptr);
		int materialID = pMaterial->GetIndexArray().GetAt(polygon_i);				//マテリアルのID番号

		//マテリアル情報が一致したらポリゴンを構成する頂点番号をインデックスに追加する
		if (materialID == material)
		{
			//3頂点分
			for (int vertex_i = POLYGON_VERTICES - 1; vertex_i >= 0; vertex_i--)
			{
				ppIndexDatas_[material][indexCount] = pMesh->GetPolygonVertex(polygon_i, vertex_i);	//頂点番号をインデックスに追加
				indexCount++;																		//インデックスのカウントを増やす
			}
		}
	}

	//マテリアルのインデックスの個数
	indexCountEachMaterial_[material] = indexCount;
}

//インデックスバッファの作成
HRESULT FbxParts::CreateIndexBuffer(int material, int indexCount)
{
	//インデックスバッファの設定
	D3D11_BUFFER_DESC bd;
	bd.Usage			= D3D11_USAGE_DEFAULT;				//バッファーの読み取りおよび書き込みが予想される方法
	bd.ByteWidth		= sizeof(int) * indexCount;			//バッファーのサイズ
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;			//バッファーがパイプラインにバインドされる方法
	bd.CPUAccessFlags	= 0;								//CPUアクセスフラグ
	bd.MiscFlags		= 0;								//その他のフラグ

	//初期化データの設定
	D3D11_SUBRESOURCE_DATA InitData;						//初期化データ
	InitData.pSysMem = ppIndexDatas_[material];				//初期化データへのポインタ

	//インデックスバッファの作成
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&bd, &InitData, &ppIndexBuffer_[material])))
	{
		MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//コンスタントバッファ準備
HRESULT FbxParts::InitConstantBuffer()
{
	//コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth			= sizeof(CONSTANT_BUFFER);		//バッファーのサイズ
	cb.Usage				= D3D11_USAGE_DYNAMIC;			//バッファーの読み取りおよび書き込みが予想される方法
	cb.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;	//バッファーがパイプラインにバインドされる方法
	cb.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;		//CPUアクセスフラグ
	cb.MiscFlags			= 0;							//その他のフラグ
	cb.StructureByteStride	= 0;							//バッファー構造内の各要素のサイズ

	//コンスタントバッファの作成
	if (FAILED(Direct3D::GetDevice()->CreateBuffer(&cb, nullptr, &pConstantBuffer_)))
	{
		MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//マテリアル準備
HRESULT FbxParts::InitMaterial(FbxNode* pNode)
{
	pMaterialDatas_ = new MATERIAL[materialCount_];	//マテリアル数分メモリ確保

	//マテリアル分ループ
	for (DWORD i = 0; i < materialCount_; i++)
	{
		FbxSurfaceMaterial* pMaterial	= pNode->GetMaterial(i);		//i番目のマテリアル情報
		FbxSurfacePhong*	pPhong		= (FbxSurfacePhong*)pMaterial;	//フォン
		FbxDouble3			ambient		= pPhong->Ambient;				//アンビエント
		FbxDouble3			diffuse		= pPhong->Diffuse;				//ディフューズ

		//クラス変数に格納
		pMaterialDatas_[i].ambient		= XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
		pMaterialDatas_[i].diffuse		= XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		pMaterialDatas_[i].specular		= XMFLOAT4(0, 0, 0, 0);
		pMaterialDatas_[i].shininess	= 0;

		//フォンシェーディングを使用しているか
		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			FbxDouble3 specular = pPhong->Specular;				//スペキュラー

			//クラス変数に格納
			pMaterialDatas_[i].specular		= XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);
			pMaterialDatas_[i].shininess	= (float)pPhong->Shininess;
		}

		//使用テクスチャの準備
		if (FAILED(InitDiffuseTexture(pMaterial, i))) return E_FAIL;

		//法線マップの準備
		if (FAILED(InitNormalTexture(pMaterial, i))) return E_FAIL;
	}

	return S_OK;
}

//テクスチャの読み込み
HRESULT FbxParts::InitDiffuseTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material)
{
	pMaterialDatas_[material].pDiffuseTexture = nullptr;										//格納用テクスチャの初期化

	//テクスチャの使用枚数を取得
	FbxProperty  property			= pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);	//ディフューズのプロパティ
	int			 fileTextureCount	= property.GetSrcObjectCount<FbxFileTexture>();				//テクスチャーの枚数

	if (!fileTextureCount) return S_OK;															//テクスチャ情報がなければ何もしない

	//テクスチャファイルまでのパスを取得
	FbxFileTexture* pTextureInfo	= property.GetSrcObject<FbxFileTexture>(0);					//テクスチャ－情報
	const char*		textureFilePath = pTextureInfo->GetRelativeFileName();						//ファイル名（パス）

	//ファイル名+拡張子だけにする
	char name[_MAX_FNAME];																		//ファイル名
	char ext[_MAX_EXT];																			//拡張子
	_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);		//ファイルパスを分割
	wsprintf(name, "%s%s", name, ext);															//ファイル名と拡張子を一つの文字列にする

	//テクスチャの読み込み
	pMaterialDatas_[material].pDiffuseTexture = new Texture;
	if (FAILED(pMaterialDatas_[material].pDiffuseTexture->Load(name))) return E_FAIL;

	return S_OK;
}

//テクスチャの読み込み
HRESULT FbxParts::InitNormalTexture(fbxsdk::FbxSurfaceMaterial* pMaterial, int material)
{
	pMaterialDatas_[material].pNormalTexture = nullptr;										//格納用テクスチャの初期化

	//テクスチャの使用枚数を取得
	FbxProperty  property = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);		//ディフューズのプロパティ
	int			 fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();			//テクスチャーの枚数

	if (!fileTextureCount) return S_OK;														//テクスチャ情報がなければ何もしない

	//テクスチャファイルまでのパスを取得
	FbxFileTexture* pTextureInfo = property.GetSrcObject<FbxFileTexture>(0);				//テクスチャ－情報
	const char* textureFilePath = pTextureInfo->GetRelativeFileName();						//ファイル名（パス）

	//ファイル名+拡張子だけにする
	char name[_MAX_FNAME];																	//ファイル名
	char ext[_MAX_EXT];																		//拡張子
	_splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);	//ファイルパスを分割
	wsprintf(name, "%s%s", name, ext);														//ファイル名と拡張子を一つの文字列にする

	//テクスチャの読み込み
	pMaterialDatas_[material].pNormalTexture = new Texture;
	if (FAILED(pMaterialDatas_[material].pNormalTexture->Load(name))) return E_FAIL;

	return S_OK;
}

//ボーンの情報を準備
void FbxParts::InitSkelton(FbxMesh* pMesh)
{
	// デフォーマ情報（ボーンとモデルの関連付け）の取得
	FbxDeformer* pDeformer = pMesh->GetDeformer(0);

	if (pDeformer == nullptr) return;					//ボーン情報がなければ何もしない

	//デフォーマ情報からスキンメッシュ情報を取得
	pSkinInfo_ = (FbxSkin *)pDeformer;

	//ボーンの数に合わせてウェイトを作成する
	CreateBoneWeight();

	//ボーンの影響力を頂点ごとに整理して格納
	SetBoneWeight();

	//ボーンを生成
	CreateBone();
}

//ボーンの数に合わせてウェイトを作成する
void FbxParts::CreateBoneWeight()
{
	//ボーン情報を取得する
	numBone_	= pSkinInfo_->GetClusterCount();	//ボーンの個数を取得する
	ppCluster_	= new FbxCluster*[numBone_];		//ボーンの個数分クラスター情報を格納するメモリを確保

	//ボーン数分のクラスター情報を取得する
	for (int i = 0; i < numBone_; i++) ppCluster_[i] = pSkinInfo_->GetCluster(i);

	//ボーンの数に合わせてウェイト情報を準備する
	pWeightArray_ = new Weight[vertexCount_];
	for (DWORD vertex_i = 0; vertex_i < vertexCount_; vertex_i++)
	{
		pWeightArray_[vertex_i].positionOrigin	= pVertexDatas_[vertex_i].position;	//変形していない状態の頂点座標
		pWeightArray_[vertex_i].normalOrigin	= pVertexDatas_[vertex_i].normal;	//変形していない状態の頂点の法線
		pWeightArray_[vertex_i].pBoneIndex		= new int[numBone_];				//ボーンの個数分インデックス情報を格納するメモリを確保
		pWeightArray_[vertex_i].pBoneWeight		= new float[numBone_];				//ボーンの個数分ウェイト情報を格納するメモリを確保

		//全ボーンのウェイト情報の初期化
		for (int bone_i = 0; bone_i < numBone_; bone_i++)
		{
			pWeightArray_[vertex_i].pBoneIndex[bone_i]	= -1;
			pWeightArray_[vertex_i].pBoneWeight[bone_i] = 0.f;
		}
	}
}

//ボーンのウェイト情報を頂点ごとに整理して格納
void FbxParts::SetBoneWeight()
{
	const int MAX_BIND_BONE = 4;			//1頂点に関連付けられるボーンの最大数

	//それぞれのボーンに影響を受ける頂点を調べる
	//それを頂点ベースでボーンインデックス・重みを整理する
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//ボーンの情報を取得
		int		indexCount	= ppCluster_[bone_i]->GetControlPointIndicesCount();//このボーンに影響を受ける頂点数
		int*	pIndex		= ppCluster_[bone_i]->GetControlPointIndices();		//ボーン/ウェイト情報の番号
		double* pWeight		= ppCluster_[bone_i]->GetControlPointWeights();		//頂点ごとのウェイト情報

		//頂点側からインデックスをたどって、頂点側で整理する
		//頂点数分ループ
		for (int vertex_i = 0; vertex_i < indexCount; vertex_i++)
		{
			//頂点に関連付けることができるボーンの最大数分ループ
			for (int bindbone_i = 0; bindbone_i < MAX_BIND_BONE; bindbone_i++)
			{
				if (bindbone_i >= numBone_)	break;						//Fbxのボーンを全て見終わったら

				//この頂点に対するウェイトが既に関連付けられたものより大きければ降順になるように入れ替える
				if (pWeight[vertex_i] > pWeightArray_[pIndex[vertex_i]].pBoneWeight[bindbone_i])
				{
					//pWeight[vertex]の格納位置以降に格納されている値の要素数を後ろにずらす
					for (int i = numBone_ - 1; i > bindbone_i; i--)
					{
						pWeightArray_[pIndex[vertex_i]].pBoneIndex[i]	= pWeightArray_[pIndex[vertex_i]].pBoneIndex[i - 1];	//インデックスを格納する要素数を１つ後ろにずらす
						pWeightArray_[pIndex[vertex_i]].pBoneWeight[i]	= pWeightArray_[pIndex[vertex_i]].pBoneWeight[i - 1];	//ウェイトを格納する要素数を１つ後ろにずらす
					}

					//ボーンを新しく関連付ける
					pWeightArray_[pIndex[vertex_i]].pBoneIndex[bindbone_i]	= bone_i;					//新しく関連付けるボーン
					pWeightArray_[pIndex[vertex_i]].pBoneWeight[bindbone_i] = (float)pWeight[vertex_i];	//新しく関連付けるボーンの頂点に対するウェイト
					break;
				}
			}
		}
	}
}

//ボーンを生成
void FbxParts::CreateBone()
{
	pBoneArray_ = new Bone[numBone_];						//ボーンの配列のメモリ確保

	//ボーンの個数分ループ
	for (int bone_i = 0; bone_i < numBone_; bone_i++)
	{
		//ボーンのデフォルト位置を取得する
		FbxAMatrix  matrix;									//Fbx形式の関節行列
		ppCluster_[bone_i]->GetTransformLinkMatrix(matrix);	//ボーンのデフォルト位置を取得

		//関節行列をFbx形式からDirectXへ変換コピーする
		XMFLOAT4X4 pose;									//DirectX形式の関節行列
		for (DWORD y = 0; y < 4; y++) {
			for (DWORD x = 0; x < 4; x++) {
				pose(x, y) = (float)matrix.Get(x, y);
			}

			//右手系(Maya)から左手系(DirectX)へ変換
			pose.m[y][0] *= -1.f;
		}

		//ボーンのデフォルト位置を格納
		pBoneArray_[bone_i].bindPose = XMLoadFloat4x4(&pose);
	}
}

//マテリアルごとに描画
void FbxParts::DrawMaterial(Transform & transform) const
{
	D3D11_MAPPED_SUBRESOURCE	data;
	ID3D11DeviceContext*		pContext = Direct3D::GetContext();	//デバイスコンテキストのポインタ

	//マテリアル分ループ
	for (DWORD material_i = 0; material_i < materialCount_; material_i++)
	{
		//インデックスバッファーをセット
		pContext->IASetIndexBuffer(ppIndexBuffer_[material_i], DXGI_FORMAT_R32_UINT, 0);

		//コンスタントバッファに渡す情報を準備
		CONSTANT_BUFFER cb;
		cb.vertexConversionMatrix	= XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
		cb.normalConversionMatrix	= XMMatrixTranspose(transform.GetRotateMatrix() * XMMatrixInverse(nullptr, transform.GetScaleMatrix()));
		cb.matWorld					= transform.GetWorldMatrix();
		cb.ambient					= pMaterialDatas_[material_i].ambient;
		cb.diffuseColor				= pMaterialDatas_[material_i].diffuse;
		cb.speculer					= pMaterialDatas_[material_i].specular;
		cb.shininess				= pMaterialDatas_[material_i].shininess;
		cb.isTexture				= pMaterialDatas_[material_i].pDiffuseTexture != nullptr;
		XMStoreFloat4(&cb.cameraPosition, Camera::GetPosition());

		//テクスチャがあれば設定
		if (cb.isTexture)
		{
			//サンプラー（テクスチャの表示の仕方）を設定
			ID3D11SamplerState*			pSampler = pMaterialDatas_[material_i].pDiffuseTexture->GetSampler();
			pContext->PSSetSamplers(0, 1, &pSampler);

			//シェーダーリソースビュー（テクスチャ）を設定
			ID3D11ShaderResourceView*	pSRV = pMaterialDatas_[material_i].pDiffuseTexture->GetSRV();
			pContext->PSSetShaderResources(0, 1, &pSRV);
		}

		//現在のシェーダが地面描画用であれば地面描画に必要な項目を設定する
		if (Direct3D::GetCurrentShaderType() == Direct3D::SHADER_TYPE::GROUND)
		{
			//ノーマルマップテクスチャをシェーダーリソースビューに設定
			ID3D11ShaderResourceView* pSRV = pMaterialDatas_[material_i].pNormalTexture->GetSRV();
			pContext->PSSetShaderResources(1, 1, &pSRV);
		}

		//コンスタントバッファの書き換え
		pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);	//GPUからのデータアクセスを止める
		memcpy_s(data.pData, data.RowPitch, (void*)(&cb), sizeof(cb));			//データ値を送る
		pContext->Unmap(pConstantBuffer_, 0);									//再開

		//ポリゴンメッシュの描画
		pContext->DrawIndexed(indexCountEachMaterial_[material_i], 0, 0);
	}
}