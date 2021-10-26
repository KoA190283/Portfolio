#pragma once
#include<string>
#include<vector>

#include"Direct3D.h"
#include"Fbx.h"
#include"Transform.h"

//モデルデータの管理
namespace Model
{
	using DirectX::XMMATRIX;
	using DirectX::XMVECTOR;

	const int ERROR_CORD = -1;	//画像データの取得に失敗した場合の番号

	//モデルデータの読み込み
	//引数：fileName　	ファイルのパス
	//戻値：ハンドル番号
	int Load(const std::string& fileName);

	//表示する状態を設定
	//引数１：handle		ハンドル番号
	//引数２：transform		表示する状態
	void SetTransform(int handle, Transform& transform);

	//描画
	//引数：handle　		ハンドル番号
	void Draw(int handle);

	//描画
	//引数１：handle　		ハンドル番号
	//引数２：type　		描画に使用するシェーダーの種類
	void Draw(int handle, Direct3D::SHADER_TYPE type);

	//指定された時間のアニメーションの状態に変形する
	//引数１：handle　		ハンドル番号
	//引数２：frame　		アニメーションのフレーム数
	void DeformAnimationPose(int handle, int frame);

	//指定された時間のアニメーションの姿勢情報を設定する（変形はしない）
	//引数１：handle　		ハンドル番号
	//引数２：frame　		アニメーションのフレーム数
	void SetAnimationPose(int handle, int frame);

	//二つの姿勢の差分を新しい姿勢として設定（変形はしない）
	//引数１：Basehandle　		新しい姿勢を設定するFBXのハンドル番号
	//引数２：materialHandle　	新しい姿勢の設定に影響を与えるFBXのハンドル番号
	//引数３：percentage　		materialHandleのFBXの影響力
	void SetDifferencePose(int Basehandle, int materialHandle, float percentage);

	//二つのモデルの初期姿勢から現在の姿勢への差分を合成し新しい姿勢として設定（変形はしない）
	//引数１：Basehandle　		新しい姿勢を設定するFBXのハンドル番号
	//引数２：materialHandle　	新しい姿勢の設定に影響を与えるFBXのハンドル番号
	void AddDifferencePose(int Basehandle, int materialHandle);

	//設定されている姿勢情報で変形する
	//引数：handle　		ハンドル番号
	void Deform(int handle);

	//ボーンの位置を取得する
	//引数１：handle　		ハンドル番号
	//引数２：boneName　	ボーンの名前
	//戻値　：ボーンの位置
	XMVECTOR GetBonePosition(int handle, const std::string& boneName);

	//ロード済みモデルのハンドル番号を取得する
	//引数：fileName　	ファイルのパス
	//戻値：ハンドル番号
	int GetHandle(const std::string& fileName);

	//解放
	void Release();

	//レイキャスト
	//引数１：handle		ハンドル番号
	//引数２：rayData		レイキャストに使用するデータ
	void RayCast(int handle, RayCastData* rayData);
};

