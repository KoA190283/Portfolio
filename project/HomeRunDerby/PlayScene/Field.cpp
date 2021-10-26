#include "../Engine/BoxCollider.h"
#include "../Engine/Model.h"
#include "../ExternalDataManager.h"
#include "Field.h"

//コンストラクタ
Field::Field(IGameObject* parent)
	: IGameObject(parent, "Field")
{
}

//初期化
void Field::Initialize()
{
	//モデルデータのロード
	const int MODEL_HANDLE_FIELD_OBJECT = Model::Load(ExternalDataManager::GetFilePathFieldObjectModel());
	assert(MODEL_HANDLE_FIELD_OBJECT != Model::ERROR_CORD);

	const int MODEL_HANDLE_GROUND		= Model::Load(ExternalDataManager::GetFilePathGroundModel());
	assert(MODEL_HANDLE_GROUND != Model::ERROR_CORD);

	//フィールドデータを設定
	ExternalDataManager::SetFieldData(DirectX::g_XMZero, DirectX::XMVectorSet(0, 0, -18.44f, 0), 100.f);

	//サイズ変更
	transform_.scale_ = DirectX::XMVectorSet(400.f, 0.00000001f, 400.f, 0);

	//当たり判定の設定追加
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;												//オブジェクトから当たり判定へのベクトル
	BoxCollider*	newCollider					= new BoxCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//新しい当たり判定
	AddCollider(newCollider);

	//更新はしない
	Inactivate();
}

//更新
void Field::Update()
{
}

//描画
void Field::Draw()
{
	//モデルデータのハンドル番号を取得
	const int MODEL_HANDLE_GROUND		= Model::GetHandle(ExternalDataManager::GetFilePathGroundModel());			//グラウンドのモデルのハンドル
	const int MODEL_HANDLE_FIELD_OBJECT = Model::GetHandle(ExternalDataManager::GetFilePathFieldObjectModel());	//フィールドオブジェクトのモデルのハンドル

	//地面の描画
	Model::SetTransform(MODEL_HANDLE_GROUND, transform_);
	Model::Draw(MODEL_HANDLE_GROUND, Direct3D::SHADER_TYPE::GROUND);

	//フィールドオブジェクトの描画
	Transform trans;
	trans.position_.vecY += 0.0001f;								//少し浮かせる
	Model::SetTransform(MODEL_HANDLE_FIELD_OBJECT, trans);
	Model::Draw(MODEL_HANDLE_FIELD_OBJECT,Direct3D::SHADER_TYPE::NO_SHADE_3D);
}

//解放
void Field::Release()
{
}