#pragma once
#include <map>
#include "../Engine/IGameObject.h"
#include "../ExternalDataManager.h"

//バッターの管理
class Batter : public IGameObject
{
	using XMVECTOR		= DirectX::XMVECTOR;
	using SWING_COURSE	= ExternalDataManager::SWING_COURSE;

private:

	XMVECTOR						prevBatPosition_;		//前フレームのバットの位置
	std::map<SWING_COURSE, float>	compositeMap_;			//合成に使用する情報 <合成するコース, 合成比率>
	int								nextAnimationFrame_;	//次に描画するアニメーションのフレーム数
	int								hModelDraw_;			//描画するモデルのハンドル
	int								hColliderBat_;			//バットの当たり判定のハンドル
	bool							isSwing_;				//スイングを行うか

public:

	//コンストラクタ
	Batter(IGameObject* parent);

	//初期化
	void Initialize() override;

	//オブジェクトに対してスイングが与える影響（ベクトル）を取得する
	//引数：hitPosition　スイングの影響をを受けるオブジェクトの位置
	//戻値：hitPositionの位置にあるオブジェクトに対して与えるスイングのベクトル
	XMVECTOR GetSwingVector(const XMVECTOR& hitPosition) const;

	//swingPositionにスイングするために必要な情報を取得して次のUpdateからスイングする
	//引数：swingPosition　スイングする位置(ストライクゾーンの真ん中基準)
	void Swing(const XMVECTOR& swingPosition);

	//アニメーションを最初の状態に戻す
	void ResetAnimation();

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;


	//ストライクゾーンの設定
	void SetStrikeZone() const;

	//バットの当たり判定の位置の設定
	void SetBatColliderPosition();

	//次フレームのアニメーションの姿勢を設定する
	void SetNextAnimationPose();

	//二つの値の中間値(平均値)を取得
	//引数１：value1　一つ目の値
	//引数２：value2　二つ目の値
	//戻値　：中間値(平均値)
	float GetMiddleValue(float value1, float value2) const;

	//二つの値のより大きいほうの値を取得
	//引数１：value1　一つ目の値
	//引数２：value2　二つ目の値
	//戻値　：大きいほうの値
	float GetLargerValue(float value1, float value2) const;

	//二つの値のより小さいほうの値を取得
	//引数１：value1　一つ目の値
	//引数２：value2　二つ目の値
	//戻値　：小さいほうの値
	float GetSmallerValue(float value1, float value2) const;
};