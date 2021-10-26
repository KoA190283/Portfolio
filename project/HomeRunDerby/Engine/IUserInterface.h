#pragma once
#include "Transform.h"
#include <string>

//UIの抽象クラス
class IUserInterface
{
protected:

	Transform transform_;				//変形情報

public:

	IUserInterface();
	virtual ~IUserInterface();

	virtual void Initialize()	= 0;	//初期化
	virtual void Update()		= 0;	//更新
	virtual void Draw()			= 0;	//描画
	virtual void Release()		= 0;	//解放
};

