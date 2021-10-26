#pragma once
#include "Transform.h"

namespace Math
{
	using DirectX::XMVECTOR;

	//三角形と線分の交差判定
	//引数１:origin			レイの始点
	//引数２:ray			レイの向きベクトル
	//引数３:vertex0		判定する三角の頂点
	//引数４:vertex1		判定する三角の頂点
	//引数５:vertex2		判定する三角の頂点
	//引数６:distance		レイの始点から交点までの距離(出力)
	//戻値:交差しているか
	bool IntersectLineToPolygon(const XMVECTOR& origin, const XMVECTOR& ray,
		const XMVECTOR& vertex0, const XMVECTOR& vertex1, const XMVECTOR& vertex2, float* distance);


	//二次元上の線分同士の交差判定
	//引数１:startPosition1	線分1の始点
	//引数２:startPosition2	線分2の始点
	//引数３:vector1		線分1のベクトル
	//引数４:vector2		線分2のベクトル
	//引数５:ratio1			線分1の内分比(出力)
	//引数６:ratio2			線分2の内分比(出力)
	//引数７:intersection	交点(出力)
	//戻値:交差しているか
	bool IntersectLineToLine2D(const XMVECTOR&	startPosition1, const XMVECTOR&	startPosition2,
		const XMVECTOR&	vector1, const XMVECTOR& vector2, float* ratio1, float* ratio2, XMVECTOR* intersection);

	//行列式	
	//引数:edge1			ベクトル1
	//引数:edge2			ベクトル2
	//引数:edge3			ベクトル3
	//戻値:拡大倍率
	float Det(const XMVECTOR& edge1, const XMVECTOR& edge2, const XMVECTOR& edge3);
}
