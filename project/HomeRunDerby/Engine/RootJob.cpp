#include "RootJob.h"

#include "SceneManager.h"

RootJob::RootJob()
{
}

RootJob::~RootJob()
{
}

//初期化
void RootJob::Initialize()
{
	//シーンマネージャーの生成
	Instantiate<SceneManager>(this);
}

//更新
void RootJob::Update()
{
}

//描画
void RootJob::Draw()
{
}

//解放
void RootJob::Release()
{
}
