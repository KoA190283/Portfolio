#pragma once
#include "IGameObject.h"

//GameObjectを継承するクラスのルートとなるオブジェクト
class RootJob :public IGameObject
{
public:

	RootJob();
	~RootJob();

	//初期化
	void Initialize()	override;

private:

	//更新
	void Update()		override;
	
	//描画
	void Draw()			override;
	
	//解放
	void Release()		override;
};

