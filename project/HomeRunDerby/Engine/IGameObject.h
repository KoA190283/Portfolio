#pragma once
#include <string>
#include <list>
#include <vector>

#include "Transform.h"
#include "ICollider.h"

//ゲームオブジェクトの抽象クラス
class IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

//変数
private:

	char state_;							//ステータス
	std::list<IGameObject*>	childList_;		//子供のリスト
	std::vector<ICollider*>	colliderList_;	//当たり判定のリスト

protected:

	std::string				objectName_;	//名前
	IGameObject*			pParent_;		//親のポインタ
	Transform				transform_;		//変形情報をまとめたもの

//関数
public:

	//コンストラクタ
	//親クラスのポインタと自分の名前を設定
	IGameObject();
	IGameObject(IGameObject* parent);
	IGameObject(IGameObject* parent, const std::string& name);

	//list<IGameObject*>から子クラスのデストラクタを呼べるようにvirtual
	virtual ~IGameObject();



	//ゲームループ
	void UpdateSub();						//自分のUpdate()と子のUpdateSub()を呼ぶ
	void DrawSub();							//自分のDraw()と子のDrawSub()を呼ぶ
	void ReleaseSub();						//自分のRelease()と子のReleaseSub()を呼ぶ



	//自分と子孫の衝突処理
	//引数:pTarget		判定を行うオブジェクト
	void CollisionSub(IGameObject* pTarget);



	//状態変化
	void KillMe();							//死亡(削除対象)にする
	void Activate();						//Updateをする
	void Inactivate();						//Updateをしない
	void Visible();							//Drawをする
	void Invisible();						//Drawをしない
	void EndGame();							//ゲームを終了する
	bool IsDead() const;					//削除するか
	bool IsActive() const;					//Update実行するか
	bool IsVisible() const;					//Draw実行するか
	bool IsEndGame() const;					//ゲームを終了するか



	//名前の一致するオブジェクトを探す
	//引数：name　オブジェクト名
	//戻値：見つかったオブジェクトのポインタ
	IGameObject* FindObject(const std::string& name);			//全てのオブジェクトから
	IGameObject* FindChildObject(const std::string& name);		//子孫から
	IGameObject* FindAncestrsObject(const std::string& name);	//先祖から


	//無引数の取得関数
	IGameObject*				GetRootJob();					//ルートのポインタ
	std::list<IGameObject*>*	GetChildList();					//子供のリスト
	IGameObject*				GetParent();					//親のポインタ
	std::string					GetName() const;				//オブジェクト名
	XMVECTOR					GetPosition() const;			//位置
	XMVECTOR					GetQuaternion() const;			//回転
	XMVECTOR 					GetScale() const;				//拡大縮小
	Transform*					GetTransform();					//変形情報


	//位置設定
	//引数:position		位置ベクトル
	//引数:x			x座標
	//引数:y			y座標
	//引数:z			z座標
	void SetPosition(const XMVECTOR& position);
	void SetPosition(float x, float y, float z);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);


	//回転設定
	//引数:rotate		回転ベクトル
	//引数:x			x軸の角度
	//引数:y			y軸の角度
	//引数:z			z軸の角度
	void SetRotate(const XMVECTOR& rotate);
	void SetRotate(float x, const float y, const float z);
	void SetRotateX(float x);
	void SetRotateY(float y);
	void SetRotateZ(float z);

	//拡大設定
	//引数:scale		拡大縮小ベクトル
	//引数:x			x軸方向の拡大率
	//引数:y			y軸方向の拡大率
	//引数:z			z軸方向の拡大率
	void SetScale(const XMVECTOR& scale);
	void SetScale(float x, float y, float z);


	//当たり判定の追加
	//引数：collider　当たり判定
	//戻値：当たり判定のハンドル
	int AddCollider(ICollider* collider);

	//全ての当たり判定の有効化
	void AllColliderActivate();

	//全ての当たり判定の無効化
	void AllColliderInactivate();

protected:

	//純粋仮想関数
	virtual void Initialize()	= 0;		//初期化
	virtual void Update()		= 0;		//更新
	virtual void Draw()			= 0;		//描画
	virtual void Release()		= 0;		//解放

	//子孫の開放
	void ReleaseAllChildren();

	//子孫の描画を行った後に行う描画処理
	virtual void DrawAfterChildren();

	//当たった時の処理
	//引数:pTarget		当たったオブジェクト
	//引数:pData		当たったときのデータ
	virtual void OnCollision(IGameObject* pTarget, CollisionData *pData);

	//当たり判定の位置を設定
	//引数１：hCollider	設定する当たり判定のハンドル
	//引数２：posision	設定する位置の配列
	void SetColliderPosition(int hCollider, XMVECTOR const* posision);

	//当たり判定の位置を設定
	//引数１：hCollider	設定する当たり判定のハンドル
	//引数２：size		当たり判定の大きさ
	void SetColliderSize(int hCollider, XMVECTOR const& size);

	//子クラスのインスタンス化
	//引数：親になるIGameObjectのポインタ
	//戻値：生成されたクラスのインスタンス
	template <class T>
	IGameObject* Instantiate(IGameObject* parent)
	{
		T* pNewObject;														//新しいオブジェクトのポインタ
		pNewObject = new T(parent);											//インスタンス化
		pNewObject->Initialize();											//初期化
		pNewObject->GetTransform()->SetParent(parent->GetTransform());		//親の変形情報を設定

		parent->GetChildList()->push_back(pNewObject);						//インスタンス化されたオブジェクトを親の子供リストに追加する
		return pNewObject;
	}

private:

	//当たり判定
	void Collision();						//同一シーン内の当たり判定
};

