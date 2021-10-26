#include "ICollider.h"

#include "Global.h"
#include "Transform.h"

//コンストラクタ
ICollider::ICollider(const XMVECTOR* position, const XMVECTOR& colliderSize, COLLIDER_TYPE type, int const positionSize):
	size_(colliderSize),
	type_(type), 
	isInactive_(false),
	isContinuous_(false)
{
	pPrevPosition_	= new XMVECTOR[positionSize];
	pNextPosition_	= new XMVECTOR[positionSize];

	positionSize_	= positionSize;

	SetPosision(position);
	SetPosision(position);
}

//デストラクタ
ICollider::~ICollider()
{
	SAFE_DELETE_ARRAY(pNextPosition_);
	SAFE_DELETE_ARRAY(pPrevPosition_);
}

//当たり判定の位置を更新
void ICollider::UpdateGameObjectPosition(const XMVECTOR& posision)
{
	prevGameObjectPosition_	= nextGameObjectPosition_;	//前フレームの位置を入れ替え
	nextGameObjectPosition_ = posision;					//次フレームの位置を取得
}

//当たり判定の座標を設定
void ICollider::SetPosision(const XMVECTOR* positionList)
{
	if (positionList == nullptr) return;		//ポインタの中身がなければ何もしない

	for (int i = 0; i < positionSize_; i++)
	{
		pPrevPosition_[i] = pNextPosition_[i];	//前の位置を入れ替え
		pNextPosition_[i] = positionList[i];	//次の位置を入れ替え
	}
}

//当たり判定の大きさを設定
void ICollider::SetSize(const XMVECTOR& size)
{
	size_ = size;
}

//当たり判定の型を取得
COLLIDER_TYPE ICollider::GetType() const
{
	return type_; 
}

//当たり判定に必要な位置情報の数
int ICollider::GetNeedPositionSize() const
{
	return positionSize_; 
}

//当たり判定の有効化
void ICollider::Activate()
{
	isInactive_ = false; 
}

//当たり判定の無効化
void ICollider::Inactivate()
{
	isInactive_ = true; 
}

//当たり判定を連続的に行う
void ICollider::Continuous()
{
	isContinuous_ = true;
}

//当たり判定を断続的に行う
void ICollider::Discrete()
{
	isContinuous_ = false; 
}

//当たり判定(球型同士)
bool ICollider::IsHitSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;
	;
	//当たり判定のどちらかが無効状態であれば判定は行わない
	if (sphere1.isInactive_ || sphere2.isInactive_)	return false;

	pData->direction					= DirectX::g_XMFltMax.v;							//当たり同士の距離を最大値に
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSpheres(sphere1, sphere2, pData, 1.f);		//次のフレームの位置の当たり判定

	//正確性を求めないならisHitNextPosを返す
	if (!(sphere1.isContinuous_ | sphere2.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//当たり判定の距離が 移動距離+判定範囲 より離れていたら当たらない
	{
		const XMVECTOR NEXT_SPHERE_POSITION_1 = sphere1.pNextPosition_[0] + sphere1.nextGameObjectPosition_;			//sphere1の次の位置
		const XMVECTOR PREV_SPHERE_POSITION_1 = sphere1.pPrevPosition_[0] + sphere1.prevGameObjectPosition_;			//sphere1の前の位置		
		const XMVECTOR NEXT_SPHERE_POSITION_2 = sphere2.pNextPosition_[0] + sphere2.nextGameObjectPosition_;			//sphere2の次の位置
		const XMVECTOR PREV_SPHERE_POSITION_2 = sphere2.pPrevPosition_[0] + sphere2.prevGameObjectPosition_;			//sphere2の前の位置

		const float MOVE_SPHERE_DISTANCE_1	  = XMVector3Length(NEXT_SPHERE_POSITION_1 - PREV_SPHERE_POSITION_1).vecX;	//sphere1の移動距離
		const float MOVE_SPHERE_DISTANCE_2	  = XMVector3Length(NEXT_SPHERE_POSITION_2 - PREV_SPHERE_POSITION_2).vecX;	//sphere2の移動距離
		const float CLLIDER_SIZE			  = sphere1.size_.vecX + sphere2.size_.vecX;								//当たり判定のサイズ
		const float POSSIBILITY_DISTANCE	  = MOVE_SPHERE_DISTANCE_1 + MOVE_SPHERE_DISTANCE_2 + CLLIDER_SIZE;			//衝突の可能性がある距離

		//衝突の可能性がなければ終わり
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//当たり同士の距離を最大値に

	//前フレームから精度分移動した状態から当たり判定
	return JudgeSpheres(sphere1, sphere2, pData, PRECITION);
}

//当たり判定(球型とカプセル型)
bool ICollider::IsHitSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;

	//当たり判定のどちらかが無効状態であれば判定は行わない
	if (sphere.isInactive_ || capsule.isInactive_)	return false;

	pData->direction					= g_XMFltMax.v;											//当たり同士の距離を最大値に
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSphereVsCapsule(sphere, capsule, pData, 1.f);	//次のフレームの位置の当たり判定

	//正確性を求めないならisHitNextPosを返す
	if (!(sphere.isContinuous_ | capsule.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//当たり判定の距離が 移動距離+判定範囲 より離れていたら当たらない
	{
		const XMVECTOR	NEXT_SPHERE_POSITION		= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;						//sphereの次の位置
		const XMVECTOR	PREV_SPHERE_POSITION		= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;						//sphereの前の位置
		const XMVECTOR	NEXT_CAPSULE_POSITION_FRONT	= capsule.pNextPosition_[0] + capsule.nextGameObjectPosition_;						//capsuleの次の位置の手前側
		const XMVECTOR	PREV_CAPSULE_POSITION_FRONT = capsule.pPrevPosition_[0] + capsule.prevGameObjectPosition_;						//capsuleの前の位置の手前側
		const XMVECTOR	NEXT_CAPSULE_POSITION_BACK	= capsule.pNextPosition_[1] + capsule.nextGameObjectPosition_;						//capsuleの次の位置の奥側
		const XMVECTOR	PREV_CAPSULE_POSITION_BACK	= capsule.pPrevPosition_[1] + capsule.prevGameObjectPosition_;						//capsuleの前の位置の奥側
	
		const float		MOVE_SPHERE_DISTANCE		= XMVector3Length(NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION).vecX;				//sphereの移動距離
		const float		MOVE_CAPSULE_DISTANCE_FRONT = XMVector3Length(NEXT_CAPSULE_POSITION_FRONT - PREV_CAPSULE_POSITION_FRONT).vecX;	//capsuleの手前側の移動距離
		const float		MOVE_CAPSULE_DISTANCE_BACK	= XMVector3Length(NEXT_CAPSULE_POSITION_BACK - PREV_CAPSULE_POSITION_BACK).vecX;	//capsuleの奥側の移動距離
		const float		MOVE_CAPSULE_DISTANCE		= MOVE_CAPSULE_DISTANCE_FRONT + MOVE_CAPSULE_DISTANCE_BACK;							//capsuleの移動距離
		const float		CLLIDER_SIZE				= sphere.size_.vecX + capsule.size_.vecX;											//二つの当たり判定のサイズ
		const float		POSSIBILITY_DISTANCE		= MOVE_SPHERE_DISTANCE + MOVE_CAPSULE_DISTANCE + CLLIDER_SIZE;						//衝突の可能性がある距離
		
		//衝突の可能性がなければ終わり
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//当たり同士の距離を最大値に

	//前フレームから精度分移動した状態から当たり判定
	return JudgeSphereVsCapsule(sphere, capsule, pData, PRECITION);
}

//当たり判定(球型と箱型)
bool ICollider::IsHitSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::XMVector3Length;
	using DirectX::g_XMFltMax;

	//当たり判定のどちらかが無効状態であれば判定は行わない
	if (sphere.isInactive_ || box.isInactive_)	return false;

	pData->direction					= g_XMFltMax.v;									//当たり同士の距離を最大値に
	const bool IS_HIT_NEXT_POSITIONS	= JudgeSphereVsBox(sphere, box, pData, 1.f);	//次のフレームの位置の当たり判定

	//正確性を求めないならisHitNextPosを返す
	if (!(sphere.isContinuous_ | box.isContinuous_)) return IS_HIT_NEXT_POSITIONS;

	//当たり判定の距離が 移動距離+判定範囲 より離れていたら当たらない
	{
		const XMVECTOR	NEXT_SPHERE_POSITION = sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;			//sphereの次の位置
		const XMVECTOR	PREV_SPHERE_POSITION = sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;			//sphereの前の位置
		const XMVECTOR	NEXT_BOX_POSITION	 = box.pNextPosition_[0] + box.nextGameObjectPosition_;					//boxの次の位置
		const XMVECTOR	PREV_BOX_POSITION	 = box.pPrevPosition_[0] + box.prevGameObjectPosition_;					//boxの前の位置

		const float		MOVE_SPHERE_DISTANCE = XMVector3Length(NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION).vecX;	//sphereの移動距離
		const float		MOVE_BOX_DISTANCE	 = XMVector3Length(NEXT_BOX_POSITION - PREV_BOX_POSITION).vecX;			//boxの移動距離
		const float		CLLIDER_SIZE		 = sphere.size_.vecX + XMVector3Length(box.size_).vecX;					//boxの移動距離

		const float		POSSIBILITY_DISTANCE = MOVE_SPHERE_DISTANCE + MOVE_BOX_DISTANCE + CLLIDER_SIZE;				//衝突の可能性がある範囲

		//衝突の可能性がなければ終わり
		if (POSSIBILITY_DISTANCE < XMVector3Length(pData->direction).vecX) return false;
	}

	pData->direction = g_XMFltMax.v;	//当たり同士の距離を最大値に

	//前フレームから精度分移動した状態から当たり判定
	return JudgeSphereVsBox(sphere, box, pData, PRECITION);
}

//判定処理(球型同士)
bool ICollider::JudgeSpheres(const ICollider& sphere1, const ICollider& sphere2, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;

	//100%を超えていれば衝突判定終了
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//前フレームからpercentageだけ進んだ状態で衝突判定
	{
		//処理フレーム前後の判定に使用する位置
		const XMVECTOR NEXT_SPHERE_POSITION_1	= sphere1.pNextPosition_[0] + sphere1.nextGameObjectPosition_;	//sphere1の次の位置
		const XMVECTOR PREV_SPHERE_POSITION_1	= sphere1.pPrevPosition_[0] + sphere1.prevGameObjectPosition_;	//sphere1の前の位置		
		const XMVECTOR NEXT_SPHERE_POSITION_2	= sphere2.pNextPosition_[0] + sphere2.nextGameObjectPosition_;	//sphere2の次の位置
		const XMVECTOR PREV_SPHERE_POSITION_2	= sphere2.pPrevPosition_[0] + sphere2.prevGameObjectPosition_;	//sphere2の前の位置
	
		//フレーム間の移動ベクトル
		const XMVECTOR MOVE_SPHERE_1			= NEXT_SPHERE_POSITION_1 - PREV_SPHERE_POSITION_1;				//sphere1の移動ベクトル
		const XMVECTOR MOVE_SPHERE_2			= NEXT_SPHERE_POSITION_2 - PREV_SPHERE_POSITION_2;				//sphere2の移動ベクトル

		//前フレームからpercentageだけ進んだ位置
		const XMVECTOR SPHERE_POSITION_1		= PREV_SPHERE_POSITION_1 + MOVE_SPHERE_1 * percentage;			//sphere1の位置
		const XMVECTOR SPHERE_POSITION_2		= PREV_SPHERE_POSITION_2 + MOVE_SPHERE_2 * percentage;			//sphere2の位置

		//前より近づいていなければ衝突しない
		const XMVECTOR COLLISION_DIRECTION		= SPHERE_POSITION_1 - SPHERE_POSITION_2;						//sphere2からsphere1への当たり判定の位置へのベクトル
		const float	   NEXT_COLLIDERS_DISTANCE	= XMVector3Length(COLLISION_DIRECTION).vecX;					//二つの当たり判定の距離
		const float	   PREV_COLLIDERS_DISTANCE	= XMVector3Length(pData->direction).vecX;						//前の二つの当たり判定の距離
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;

		//当たり判定のデータを更新
		pData->positionList = { SPHERE_POSITION_1,SPHERE_POSITION_2};											//当たり判定の位置
		pData->direction	= COLLISION_DIRECTION;																//当たり判定間のベクトル

		//当たり判定同士の距離が当たり判定のサイズより短ければ当たっている
		float COLLIDER_SIZE = sphere1.size_.vecX + sphere2.size_.vecX;											//二つの当たり判定のサイズ
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE) return true;
	}

	//精度分進めて再度判定
	return JudgeSpheres(sphere1, sphere2, pData, percentage + PRECITION);
}

//判定処理(球型とカプセル型) 
bool ICollider::JudgeSphereVsCapsule(const ICollider& sphere, const ICollider& capsule, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;
	using DirectX::XMVector3Normalize;

	//100%を超えていれば衝突判定終了
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//前フレームからpercentageだけ進んだ状態で衝突判定
	{
		//処理フレーム前後の判定に使用する位置
		const XMVECTOR	NEXT_SPHERE_POSITION		= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;					//sphere1の次の位置
		const XMVECTOR	PREV_SPHERE_POSITION		= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;					//sphere1の前の位置		
		const XMVECTOR	NEXT_CAPSULE_POSITION_FRONT	= capsule.pNextPosition_[0] + capsule.nextGameObjectPosition_;					//capsuleの次の位置の手前側
		const XMVECTOR	PREV_CAPSULE_POSITION_FRONT = capsule.pPrevPosition_[0] + capsule.prevGameObjectPosition_;					//capsuleの前の位置の手前側
		const XMVECTOR	NEXT_CAPSULE_POSITION_BACK	= capsule.pNextPosition_[1] + capsule.nextGameObjectPosition_;					//capsuleの次の位置の奥側
		const XMVECTOR	PREV_CAPSULE_POSITION_BACK	= capsule.pPrevPosition_[1] + capsule.prevGameObjectPosition_;					//capsuleの前の位置の奥側

		//フレーム間の移動ベクトル
		const XMVECTOR	MOVE_SPHERE					= NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION;									//sphereの移動ベクトル
		const XMVECTOR	MOVE_CAPSULE_FRONT			= PREV_CAPSULE_POSITION_FRONT - PREV_CAPSULE_POSITION_FRONT;					//capsuleの手前側の移動ベクトル
		const XMVECTOR	MOVE_CAPSULE_BACK			= NEXT_CAPSULE_POSITION_BACK - PREV_CAPSULE_POSITION_BACK;						//capsuleの奥側の移動ベクトル

		//前フレームからpercentageだけ進んだ位置
		const XMVECTOR	SPHERE_POSITION				= PREV_SPHERE_POSITION + MOVE_SPHERE * percentage;								//sphereの位置
		const XMVECTOR	CAPSULE_POSITION_FRONT		= PREV_CAPSULE_POSITION_FRONT + MOVE_CAPSULE_FRONT * percentage;				//capsuleの手前側の位置
		const XMVECTOR	CAPSULE_POSITION_BACK		= PREV_CAPSULE_POSITION_BACK + MOVE_CAPSULE_BACK * percentage;					//capsuleの奥側の位置

		//capsulePos1を基準にしたベクトル
		const XMVECTOR	TO_SPHERE					= SPHERE_POSITION - CAPSULE_POSITION_FRONT;										//SPHERE_POSITIONへのベクトル
		const XMVECTOR	TO_CAPSULE_BACK				= CAPSULE_POSITION_BACK - CAPSULE_POSITION_FRONT;								//CAPSULE_POSITION_BACKへのベクトル
		
		//正規化
		const XMVECTOR	TO_SPHERE_DIRECTION			= XMVector3Normalize(TO_SPHERE);												//TO_SPHEREを正規化したもの
		const XMVECTOR	TO_CAPSULE_BACK_DIRECTION	= XMVector3Normalize(TO_CAPSULE_BACK);											//TO_CAPSULE_BACKを正規化したもの

		//capsuleの直線上にあるsphereと最も近い位置を計算
		const float		DOT							= DirectX::XMVector3Dot(TO_SPHERE_DIRECTION, TO_CAPSULE_BACK_DIRECTION).vecX;	//ボールへのベクトルとカプセルの奥の位置へのベクトルの内積
		XMVECTOR		collisionPosition			= TO_CAPSULE_BACK_DIRECTION * XMVector3Length(TO_CAPSULE_BACK) * DOT;			//capsuleの直線上のsphereと最も近い位置
		
		//sphereがCAPSULE_POSITION_FRONTより手前にあるなら当たり判定の位置を基準点に
		if (DOT < 0) collisionPosition	= DirectX::g_XMZero;

		//capsuleの当たり判定の位置がcapsuleベクトルの範囲外ならcapsuleの当たり判定の位置をcapsuleベクトルの先端位置に
		const bool IS_RANGE = 0 < XMVector3Length(TO_CAPSULE_BACK).vecX - XMVector3Length(collisionPosition).vecX;					//capsuleの当たり判定ベクトルの範囲内か
		if (!IS_RANGE) collisionPosition = TO_CAPSULE_BACK;

		//前より近づいていなければそれ以降は衝突しない
		const XMVECTOR	COLLISION_DIRECTION			= collisionPosition - TO_SPHERE;												//capsuleからsphereへの当たり判定の位置へのベクトル
		const float		NEXT_COLLIDERS_DISTANCE		= XMVector3Length(COLLISION_DIRECTION).vecX;									//二つの当たり判定の距離
		const float		PREV_COLLIDERS_DISTANCE		= XMVector3Length(pData->direction).vecX;										//前の二つの当たり判定の距離
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;
			
		//当たり判定のデータを更新
		pData->positionList = { SPHERE_POSITION,CAPSULE_POSITION_FRONT,CAPSULE_POSITION_BACK};										//当たり判定の位置
		pData->direction	= COLLISION_DIRECTION;																					//当たり判定間のベクトル
			
		//当たり判定同士の距離が当たり判定のサイズより短ければ当たっている
		const float COLLIDER_SIZE = sphere.size_.vecX + capsule.size_.vecX;															//二つの当たり判定のサイズ
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE) return true;
	}

	//精度分進めて再度判定
	return JudgeSphereVsCapsule(sphere, capsule, pData, percentage + PRECITION);
}

bool ICollider::JudgeSphereVsBox(const ICollider& sphere, const ICollider& box, CollisionData* pData, float percentage) const
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::XMVector3Length;

	//100%を超えていれば衝突判定終了
	const float PERCENTAGE_LIMIT = 1.f;
	if (percentage > PERCENTAGE_LIMIT)	return false;

	//前フレームからpercentageだけ進んだ状態で衝突判定
	{
		//処理フレーム前後の判定に使用する位置
		const XMVECTOR	NEXT_SPHERE_POSITION	= sphere.pNextPosition_[0] + sphere.nextGameObjectPosition_;	//sphereの次の位置
		const XMVECTOR	PREV_SPHERE_POSITION	= sphere.pPrevPosition_[0] + sphere.prevGameObjectPosition_;	//sphereの前の位置
		const XMVECTOR	NEXT_BOX_POSITION		= box.pNextPosition_[0] + box.nextGameObjectPosition_;			//boxの次の位置
		const XMVECTOR	PREV_BOX_POSITION		= box.pPrevPosition_[0] + box.prevGameObjectPosition_;			//boxの前の位置

		//フレーム間の移動ベクトル
		const XMVECTOR	MOVE_SPHERE = NEXT_SPHERE_POSITION - PREV_SPHERE_POSITION;								//sphereの移動距離
		const XMVECTOR	MOVE_BOX	= NEXT_BOX_POSITION - PREV_BOX_POSITION;									//boxの移動距離

		//前フレームからpercentageだけ進んだ位置
		const XMVECTOR	SPHERE_POSITION = PREV_SPHERE_POSITION + MOVE_SPHERE * percentage;						//sphereの位置
		const XMVECTOR	BOX_POSITION	= PREV_BOX_POSITION + MOVE_BOX * percentage;							//boxの先端の位置

		//箱型当たり判定の範囲
		XMVECTOR boxMax, boxMin;
		boxMax.vecX = BOX_POSITION.vecX + box.size_.vecX;														//boxのX方向の判定範囲
		boxMax.vecY = BOX_POSITION.vecY + box.size_.vecY;														//boxのY方向の判定範囲		
		boxMax.vecZ = BOX_POSITION.vecZ + box.size_.vecZ;														//boxのZ方向の判定範囲	
		boxMin.vecX = BOX_POSITION.vecX - box.size_.vecX;														//boxのX方向の判定範囲
		boxMin.vecY = BOX_POSITION.vecY - box.size_.vecY;														//boxのY方向の判定範囲		
		boxMin.vecZ = BOX_POSITION.vecZ - box.size_.vecZ;														//boxのZ方向の判定範囲	

		XMVECTOR collisionPosition = SPHERE_POSITION;															//box面上のcollisionPosから最も近い点															

		//collisionPosがbox面上にあるかを確認する
		XMVECTOR boxLimit[2] = { boxMin, boxMax };
		if (collisionPosition.vecX < boxMin.vecX || boxMax.vecX < collisionPosition.vecX)						//X成分
			collisionPosition.vecX = boxLimit[0 < collisionPosition.vecX].vecX;

		if (collisionPosition.vecZ < boxMin.vecZ || boxMax.vecZ < collisionPosition.vecZ)						//Z成分
			collisionPosition.vecZ = boxLimit[0 < collisionPosition.vecZ].vecZ;

		if (collisionPosition.vecY < boxMin.vecY || boxMax.vecY < collisionPosition.vecY)						//Y成分
			collisionPosition.vecY = boxLimit[0 < collisionPosition.vecY].vecY;

		//前より近づいていなければそれ以降は衝突しない
		const XMVECTOR	COLLISION_DIRECTION		= collisionPosition - SPHERE_POSITION;							//sphereからcapsuleへの当たり判定の位置へのベクトル
		const float		NEXT_COLLIDERS_DISTANCE = XMVector3Length(COLLISION_DIRECTION).vecX;					//二つの当たり判定の距離
		const float		PREV_COLLIDERS_DISTANCE = XMVector3Length(pData->direction).vecX;						//前の二つの当たり判定の距離
		if (PREV_COLLIDERS_DISTANCE <= NEXT_COLLIDERS_DISTANCE) return false;

		//当たり判定のデータを更新
		pData->positionList = { SPHERE_POSITION,BOX_POSITION };													//当たり判定の位置
		pData->direction	= COLLISION_DIRECTION;																//当たり判定間のベクトル

		//当たり判定同士の距離が当たり判定のサイズより短ければ当たっている
		const float COLLIDER_SIZE = sphere.size_.vecX;															//二つの当たり判定のサイズ
		if (NEXT_COLLIDERS_DISTANCE <= COLLIDER_SIZE)
		{
			return true;
		}
	}

	//精度分進めて再度判定
	return JudgeSphereVsBox(sphere, box, pData, percentage + PRECITION);
}