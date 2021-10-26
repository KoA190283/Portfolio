#include "IGameObject.h"

#include "Global.h"

/*
	IGameObject::state_のデータ格納について

	//ビット数：管理するフラグ

	1:オブジェクトを削除するか
	2:更新を行うか
	3:描画を行うか
	4:ゲームを終了するか

	5:空データ
	6:空データ
	7:空データ
	8:空データ
*/

const char DEAD		 = 0x01;	//死んでる
const char INACTIVE	 = 0x02;	//更新しない
const char INVISIBLE = 0x04;	//描画しない
const char END_GAME	 = 0x08;	//ゲーム終了

using DirectX::XMVECTOR;

//コンストラクタ(引数なし)
//コンストラクタを(nullptr,"")を引数にして呼び出す
IGameObject::IGameObject():IGameObject(nullptr,"")
{
}
//コンストラクタ(引数：親ポインタ)
//コンストラクタを(親ポインタ,"")を引数にして呼び出す
IGameObject::IGameObject(IGameObject* parent) :
	IGameObject(parent, "")
{
}
//コンストラクタ(引数：親ポインタ、オブジェクト名)
//親クラスのポインタと自分の名前を設定
IGameObject::IGameObject(IGameObject* parent,std::string const & name) :
	pParent_(parent), objectName_(name), state_(0)
{
	//リストの初期化
	colliderList_.clear();
	childList_.clear();	
}

//デストラクタ
IGameObject::~IGameObject()
{
}

//自分のUpdate()と子のUpdateSub()を呼ぶ
void IGameObject::UpdateSub()
{
	//更新しない
	if (!IsActive()) return;

	//更新
	Update();

	//当たり判定の位置を更新
	for (auto collider : colliderList_) collider->UpdateGameObjectPosition(transform_.position_);

	//childList_のUpdateSubを呼び出し
	for (auto child : childList_) child->UpdateSub();

	//更新後の処理
	auto it = childList_.begin();
	while (it != childList_.end())
	{
		//死んでいる
		if ((*it)->IsDead())
		{
			//死んだ子孫の解放
			(*it)->ReleaseSub();
			SAFE_DELETE(*it);
			it = childList_.erase(it);
		}
		//生きている
		else
		{
			//当たり判定
			(*it)->Collision();
			it++;
		}
	}

	//子供の当たり判定の位置を更新
	for (auto child : childList_) {
		for (auto collider : child->colliderList_)
			collider->UpdateGameObjectPosition(child->transform_.position_);
	}
}

//自分のDraw()と子のDrawSub()を呼ぶ
void IGameObject::DrawSub()
{
	//描画しない
	if (!IsVisible()) return;

	//描画
	Draw();

	//childList_のDrawSubを呼び出し
	for (auto child : childList_) child->DrawSub();

	//子孫の描画を行った後に行う描画処理
	DrawAfterChildren();
}

//自分のRelease()と子のReleaseSub()を呼ぶ
void IGameObject::ReleaseSub()
{
	//当たり判定の解放
	for (auto collider : colliderList_) SAFE_DELETE(collider);

	colliderList_.clear();

	//childList_のReleaseSubを呼び出し
	ReleaseAllChildren();

	//解放
	Release();
}

//自分と子孫の衝突処理
void IGameObject::CollisionSub(IGameObject* pTarget)
{
	//子供がいれば
	if (!childList_.empty())
	{
		//子供の当たり判定
		for (auto child : childList_) child->CollisionSub(pTarget);
	}

	//同じオブジェクト同士なら判定しない
	if (this == pTarget)  return;

	//当たり判定をもっていなければ判定しない
	if (colliderList_.empty()) return;

	//自分の持つ当たり判定分ループ
	for (auto collider : colliderList_) {

		//ターゲットの持つ当たり判定分ループ
		for (auto targetCollider : pTarget->colliderList_) {

			CollisionData pData;

			//当たってたら
			if (collider->IsHit(*targetCollider, &pData)) {

				//当たった時の処理
				OnCollision(pTarget, &pData);
			}
		}
	}

}

//自分の状態を死亡にする
void IGameObject::KillMe()
{
	state_ |= DEAD; 
}

//Updateをする
void IGameObject::Activate() 
{ 
	state_ &= !INACTIVE;
}

//Updateをしない
void IGameObject::Inactivate() 
{
	state_ |= INACTIVE; 
}

//Drawをする
void IGameObject::Visible() 
{
	state_ &= !INVISIBLE;
}

//Drawをしない
void IGameObject::Invisible() 
{
	state_ |= INVISIBLE;
}

//ゲームを終了する
void IGameObject::EndGame() 
{
	GetRootJob()->state_ |= END_GAME;
}

//死んでるか
bool IGameObject::IsDead() const
{
	return state_ & DEAD;
}

//Update実行するか
bool IGameObject::IsActive() const
{
	return !(state_ & INACTIVE);
}

//Draw実行するか
bool IGameObject::IsVisible() const
{
	return !(state_ & INVISIBLE); 
}

//ゲームを終了するか
bool IGameObject::IsEndGame() const
{
	return state_ & END_GAME;
}

//全てのオブジェクトから名前の一致するものを探す
IGameObject* IGameObject::FindObject(const std::string & name)
{
	return GetRootJob()->FindChildObject(name);
}

//名前の一致する子孫を探す
IGameObject* IGameObject::FindChildObject(const std::string & name)
{
	size_t size = childList_.size();

	//子供を探す
	for(auto child: childList_)
	{
		//見つけたらポインタを返す
		if (child->objectName_ == name) return child;

		//孫を探す
		IGameObject* obj = (child)->FindChildObject(name);
		
		//孫がいたら孫を返す
		if (obj) return obj;
	}

	//見つからなかったらnullptrを返す
	return nullptr;
}

//名前の一致する先祖を探す
IGameObject* IGameObject::FindAncestrsObject(const std::string& name)
{
	//名前が合ったら自分を返す
	if (objectName_ == name) return this;
	
	//親がいたら親を探して返す
	else if (pParent_ != nullptr) return pParent_->FindAncestrsObject(name);
	
	//いなかったらnullptrを返す
	else return nullptr;
}

//ルートのポインタを取得
IGameObject* IGameObject::GetRootJob()
{
	//親がいなかったら自分を返す
	if (pParent_ == nullptr) return this;

	//いたら親の親を探す
	else return pParent_ -> GetRootJob();
}

//子供のリスト
std::list<IGameObject*>* IGameObject::GetChildList()
{
	return &childList_; 
}				

//親のポインタ
IGameObject* IGameObject::GetParent()
{
	return pParent_;
}				

//オブジェクト名
std::string	IGameObject::GetName() const
{
	return objectName_; 
}				

//位置
XMVECTOR IGameObject::GetPosition() const
{
	return transform_.position_; 
}	

//回転
XMVECTOR IGameObject::GetQuaternion() const
{
	return transform_.quaternion_; 
}	

//拡大縮小
XMVECTOR IGameObject::GetScale() const
{
	return transform_.scale_;
}		

//変形情報
Transform* IGameObject::GetTransform()
{
	return &transform_; 
}				

//位置設定
void IGameObject::SetPosition(const XMVECTOR& position) 
{
	transform_.position_ = position;
}

//位置設定
void IGameObject::SetPosition(float x, float y, float z) 
{
	SetPosition(XMVECTOR{ x, y, z }); 
}

//xベクトルの位置設定
void IGameObject::SetPositionX(float x)
{
	transform_.position_.vecX = x;
}

//yベクトルの位置設定
void IGameObject::SetPositionY(float y)
{
	transform_.position_.vecY = y;
}

//zベクトルの位置設定
void IGameObject::SetPositionZ(float z)
{
	transform_.position_.vecZ = z;
}

//回転設定
void IGameObject::SetRotate(const XMVECTOR& rotate) 
{
	transform_.SetRotate(rotate); 
}

//回転設定
void IGameObject::SetRotate(float x, float y, float z)
{
	SetRotate(XMVECTOR{ x, y, z }); 
}

//xベクトルの回転設定
void IGameObject::SetRotateX(float x) 
{
	SetRotate(XMVECTOR{ x, 0, 0 }); 
}

//yベクトルの回転設定
void IGameObject::SetRotateY(float y) 
{
	SetRotate(XMVECTOR{ 0, y, 0 });
}

//zベクトルの回転設定
void IGameObject::SetRotateZ(float z) 
{
	SetRotate(XMVECTOR{ 0, 0, z }); 
}

//拡大設定
void IGameObject::SetScale(const XMVECTOR& scale) 
{
	transform_.scale_ = scale; 
}

//拡大設定
void IGameObject::SetScale(float x, float y, float z)
{
	SetScale(DirectX::XMVectorSet(x, y, z, 0)); 
}

//当たり判定の追加
int IGameObject::AddCollider(ICollider* collider)
{
	//当たり判定のリストに追加
	colliderList_.push_back(collider);

	//現在の自分の位置を教える
	collider -> UpdateGameObjectPosition(transform_.position_);

	//ハンドルを返す
	return (int)colliderList_.size() - 1;
}

//全ての当たり判定の有効化
void IGameObject::AllColliderActivate()
{
	if (colliderList_.empty()) return;

	for (auto collider : colliderList_)	collider->Activate();
}

//全ての当たり判定の無効化
void IGameObject::AllColliderInactivate()
{
	if (colliderList_.empty()) return;

	for (auto collider : colliderList_)	collider->Inactivate();
}

//子孫の解放
void IGameObject::ReleaseAllChildren()
{
	//childList_の解放
	for (auto child : childList_)
	{
		child->ReleaseSub();
		SAFE_DELETE(child);
	}
	childList_.clear();
}

//子孫の描画を行った後に行う描画処理
void IGameObject::DrawAfterChildren()
{
}

//当たった時の処理
void IGameObject::OnCollision(IGameObject* pTarget, CollisionData *pData)
{
}

//当たり判定の位置を設定
void IGameObject::SetColliderPosition(int hCollider, XMVECTOR const* posision)
{
	if (hCollider >= colliderList_.size())	return;	//指定番号の当たり判定が存在しなければ何もしない
	if (posision == nullptr)				return;	//引数配列ポインタの値がnulptrなら何もしない

	//当たり判定の位置を設定
	colliderList_[hCollider]->SetPosision(posision);
}

//当たり判定の位置を設定
void IGameObject::SetColliderSize(int hCollider, XMVECTOR const& size)
{
	if (hCollider >= colliderList_.size())	return;	//指定番号の当たり判定が存在しなければ何もしない

	//当たり判定の大きさを設定
	colliderList_[hCollider]->SetSize(size);
}

//同一シーン内の当たり判定
void IGameObject::Collision()
{
	//当たり判定をもってなければ何もしない
	if (colliderList_.empty()) return;

	pParent_->CollisionSub(this);
}