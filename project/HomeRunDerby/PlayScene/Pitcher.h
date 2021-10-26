#pragma once
#include "../Engine/IGameObject.h"

//ピッチャーの管理
class Pitcher : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	int			nextAnimationFrame_;	//現在のアニメーションのフレーム数
	int			ballType_;				//選択されている球種の番号
	XMVECTOR	aimPosition_;			//狙う位置

public:

	//コンストラクタ
	Pitcher(IGameObject* parent);

	//初期化
	void Initialize() override;

	//取得関数
	int			GetBallType() const;			//選択されている球種
	XMVECTOR	GetAimPosition() const;			//狙う位置
	XMVECTOR	GetReleasePosition();			//ボールを放す位置

	//ボールを投げる準備
	void InitializePitch();

	//アニメーションを最初の状態に戻す
	void ResetAnimation();

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;

	//どこに投げるかを決める
	//戻り値：選択された位置がストライクゾーンの中心からどれだけ離れているか
	XMVECTOR SelectPitchCourse() const;

	//球種の選択
	//戻値：選択された球種の番号
	int SelectBallType() const;

	//0~1の乱数を取得
	//戻値：0~1の乱数
	float GetRandomPercentage() const;

	//km/hをm/fに単位変換
	float KilometreParHourToMetreParFrame(float numeric) const;

	//*/minを*/fに単位変換
	float MinutesToFrame(float numeric) const;
};