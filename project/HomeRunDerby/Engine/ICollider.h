#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <list>

//当たり判定の型
enum class COLLIDER_TYPE 
{
	SPHERE = 0,				//球
	CAPSULE,				//カプセル
	BOX,					//箱
	MAX
};

//衝突時のデータ
//(同型：自分　別型：COLLIDER_TYPEで先に定義された方) から
struct CollisionData 
{
	using XMVECTOR = DirectX::XMVECTOR;

	std::list<XMVECTOR>	 positionList;	//自分と相手の衝突時の位置　(同型：自分　別型：COLLIDER_TYPEで先に定義された方) から格納
	XMVECTOR direction;					//(同型：自分　別型：COLLIDER_TYPEで先に定義された方) から 相手へのベクトル

	CollisionData():direction(DirectX::g_XMZero){}
};

const float PRECITION = 0.01f;			//連続的に当たり判定を行う場合の精度

//当たり判定の抽象クラス
class ICollider
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMVECTOR*				pPrevPosition_;				//オブジェクトから見た当たり判定の位置
	XMVECTOR*				pNextPosition_;				//オブジェクトから見た当たり判定の位置
	XMVECTOR				prevGameObjectPosition_;	//前フレームの当たり判定の位置
	XMVECTOR				nextGameObjectPosition_;	//次フレームの当たり判定の位置
	XMVECTOR				size_;						//当たり判定の大きさ
	COLLIDER_TYPE			type_;						//当たり判定の型
	int						positionSize_;				//判定に使用する位置の数
	bool					isInactive_;				//当たり判定を行うか
	bool					isContinuous_;				//連続的に当たり判定を行うか（すり抜け防止）

public:

	//コンストラクタ
	//引数１：position　	当たり判定の位置
	//引数２：colliderSize　当たり判定の大きさ（半径）
	//引数３：type　		当たり判定の型
	//引数４：posisionSize　判定に使用する位置の数
	ICollider(XMVECTOR const* position, XMVECTOR const& colliderSize, COLLIDER_TYPE type, int const positionSize);

	//デストラクタ
	virtual ~ICollider();

	//純粋仮想関数
	//当たったかどうか
	//引数１：pData			衝突判定の情報
	//引数２：target　		判定する相手
	//戻値：当たったか
	virtual bool IsHit(const ICollider& target, CollisionData* pData) = 0;

	//当たり判定の位置を更新
	//引数：position		位置
	void UpdateGameObjectPosition(const XMVECTOR& posision);

	//当たり判定の座標を設定
	//引数：positionList	位置の配列
	void SetPosision(const XMVECTOR* positionList);

	//当たり判定の大きさを設定
	//引数：size			
	void SetSize(const XMVECTOR& size);

	//当たり判定の型を取得
	//戻値：当たり判定の型
	COLLIDER_TYPE GetType() const;

	//当たり判定に必要な位置情報の数を取得
	//戻値：当たり判定に必要な位置情報の数
	int GetNeedPositionSize() const;

	//当たり判定の有効化
	void Activate();

	//当たり判定の無効化
	void Inactivate();

	//当たり判定を連続的に行う
	void Continuous();

	//当たり判定を断続的に行う
	void Discrete();

protected:

	//当たり判定
	//引数１：判定する当たり判定１
	//引数２：判定する当たり判定２
	//引数３：pData			判定時の情報
	//戻値：当たったか
	bool IsHitSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData) const;							//球型同士
	bool IsHitSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData) const;						//球型とカプセル型
	bool IsHitSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData) const;								//球型と床型
	
private:

	//当たり判定処理
	//引数１：判定する当たり判定１
	//引数２：判定する当たり判定２
	//引数３：pData			判定時の情報
	//引数４：percentage	判定フレーム間の移動割合
	//戻値：当たったか
	bool JudgeSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData, float percentage) const;			//球型同士
	bool JudgeSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData, float percentage) const;	//球型とカプセル型
	bool JudgeSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData, float percentage) const;			//球型と箱型

};

