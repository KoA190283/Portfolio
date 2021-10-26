#include "Math.h"

namespace Math
{
	//三角形と線分の交差判定
	bool IntersectLineToPolygon(const XMVECTOR& origin, const XMVECTOR& ray,
		const XMVECTOR& vertex0, const XMVECTOR& vertex1, const XMVECTOR& vertex2, float* distance)
	{
		//クラメルの公式を基に交差判定を行う
		
		using DirectX::operator-;

		const XMVECTOR	TO_VERTEX_1		= vertex1 - vertex0;									//頂点v0 -> 頂点v1 へのベクトル
		const XMVECTOR	TO_VERTEX_2		= vertex2 - vertex0;									//頂点v0 -> 頂点v2 へのベクトル
		const XMVECTOR	TO_ORIGIN		= origin - vertex0;										//頂点v0 -> レイの始点へのベクトル
		const XMVECTOR	RAY_DIRECTION	= DirectX::XMVector3Normalize(-ray);					//交点から始点への向きベクトル

		const float		DETERMINANT		= Math::Det(TO_VERTEX_1, TO_VERTEX_2, RAY_DIRECTION);	//行列式

		//レイと三角が平行なら交差しない
		if (DETERMINANT < 0) return false;

		const float U = Math::Det(TO_ORIGIN  , TO_VERTEX_2, RAY_DIRECTION) / DETERMINANT;
		const float V = Math::Det(TO_VERTEX_1, TO_ORIGIN  , RAY_DIRECTION) / DETERMINANT;
		const float T = Math::Det(TO_VERTEX_1, TO_VERTEX_2, TO_ORIGIN	 ) / DETERMINANT;

		//交差していない
		if (U < 0 || V < 0 || (U + V) > 1.f || T < 0)	return false;

		//出力用のメモリがあれば値を入れる
		if (distance != nullptr)	*distance = T;

		return true;
	}

	//二次元上の線分同士の交差判定
	bool IntersectLineToLine2D(const XMVECTOR&	startPosition1, const XMVECTOR&	startPosition2,
		const XMVECTOR&	vector1, const XMVECTOR& vector2, float* ratio1, float* ratio2, XMVECTOR* intersection)
	{
		using DirectX::operator+;
		using DirectX::operator-;
		using DirectX::operator*;

		XMVECTOR START_POSITION		= startPosition2 - startPosition1;							//始点
		XMVECTOR CROSS_VECTOR_1_2	= DirectX::XMVector2Cross(vector1, vector2);				//線分と線分の外積

		//線分同士が平行なら交差しない
		if (CROSS_VECTOR_1_2.vecX == 0.f) return false;

		//始点との外積
		float CROSS_START_VECTOR_1 = DirectX::XMVector2Cross(START_POSITION, vector1).vecX;		//始点と線分1
		float CROSS_START_VECTOR_2 = DirectX::XMVector2Cross(START_POSITION, vector2).vecX;		//始点と線分2

		//内分比
		float RATIO_1 = CROSS_START_VECTOR_2 / CROSS_VECTOR_1_2.vecX;							//線分1の内分比
		float RATIO_2 = CROSS_START_VECTOR_1 / CROSS_VECTOR_1_2.vecX;							//線分2の内分比

		//交差していない
		if (RATIO_1 < 0 || RATIO_1 > 1 || RATIO_2 < 0 || RATIO_2 > 1) return false;

		//出力用のメモリがあれば値を入れる
		if (ratio1 != nullptr)			*ratio1			= RATIO_1;								//線分1の内分比
		if (ratio2 != nullptr)			*ratio2			= RATIO_2;								//線分2の内分比
		if (intersection != nullptr)	*intersection	= startPosition1 + vector1 * RATIO_1;	//交点の位置

		return true;
	}

	//行列式
	float Det(const XMVECTOR& edge1, const XMVECTOR& edge2, const XMVECTOR& edge3)
	{
		return(edge1.vecX * edge2.vecY * edge3.vecZ)
			+ (edge1.vecY * edge2.vecZ * edge3.vecX)
			+ (edge1.vecZ * edge2.vecX * edge3.vecY)
			- (edge1.vecX * edge2.vecZ * edge3.vecY)
			- (edge1.vecY * edge2.vecX * edge3.vecZ)
			- (edge1.vecZ * edge2.vecY * edge3.vecX);
	}
}