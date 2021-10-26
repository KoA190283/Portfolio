#include "Math.h"

namespace Math
{
	//�O�p�`�Ɛ����̌�������
	bool IntersectLineToPolygon(const XMVECTOR& origin, const XMVECTOR& ray,
		const XMVECTOR& vertex0, const XMVECTOR& vertex1, const XMVECTOR& vertex2, float* distance)
	{
		//�N�������̌�������Ɍ���������s��
		
		using DirectX::operator-;

		const XMVECTOR	TO_VERTEX_1		= vertex1 - vertex0;									//���_v0 -> ���_v1 �ւ̃x�N�g��
		const XMVECTOR	TO_VERTEX_2		= vertex2 - vertex0;									//���_v0 -> ���_v2 �ւ̃x�N�g��
		const XMVECTOR	TO_ORIGIN		= origin - vertex0;										//���_v0 -> ���C�̎n�_�ւ̃x�N�g��
		const XMVECTOR	RAY_DIRECTION	= DirectX::XMVector3Normalize(-ray);					//��_����n�_�ւ̌����x�N�g��

		const float		DETERMINANT		= Math::Det(TO_VERTEX_1, TO_VERTEX_2, RAY_DIRECTION);	//�s��

		//���C�ƎO�p�����s�Ȃ�������Ȃ�
		if (DETERMINANT < 0) return false;

		const float U = Math::Det(TO_ORIGIN  , TO_VERTEX_2, RAY_DIRECTION) / DETERMINANT;
		const float V = Math::Det(TO_VERTEX_1, TO_ORIGIN  , RAY_DIRECTION) / DETERMINANT;
		const float T = Math::Det(TO_VERTEX_1, TO_VERTEX_2, TO_ORIGIN	 ) / DETERMINANT;

		//�������Ă��Ȃ�
		if (U < 0 || V < 0 || (U + V) > 1.f || T < 0)	return false;

		//�o�͗p�̃�����������Βl������
		if (distance != nullptr)	*distance = T;

		return true;
	}

	//�񎟌���̐������m�̌�������
	bool IntersectLineToLine2D(const XMVECTOR&	startPosition1, const XMVECTOR&	startPosition2,
		const XMVECTOR&	vector1, const XMVECTOR& vector2, float* ratio1, float* ratio2, XMVECTOR* intersection)
	{
		using DirectX::operator+;
		using DirectX::operator-;
		using DirectX::operator*;

		XMVECTOR START_POSITION		= startPosition2 - startPosition1;							//�n�_
		XMVECTOR CROSS_VECTOR_1_2	= DirectX::XMVector2Cross(vector1, vector2);				//�����Ɛ����̊O��

		//�������m�����s�Ȃ�������Ȃ�
		if (CROSS_VECTOR_1_2.vecX == 0.f) return false;

		//�n�_�Ƃ̊O��
		float CROSS_START_VECTOR_1 = DirectX::XMVector2Cross(START_POSITION, vector1).vecX;		//�n�_�Ɛ���1
		float CROSS_START_VECTOR_2 = DirectX::XMVector2Cross(START_POSITION, vector2).vecX;		//�n�_�Ɛ���2

		//������
		float RATIO_1 = CROSS_START_VECTOR_2 / CROSS_VECTOR_1_2.vecX;							//����1�̓�����
		float RATIO_2 = CROSS_START_VECTOR_1 / CROSS_VECTOR_1_2.vecX;							//����2�̓�����

		//�������Ă��Ȃ�
		if (RATIO_1 < 0 || RATIO_1 > 1 || RATIO_2 < 0 || RATIO_2 > 1) return false;

		//�o�͗p�̃�����������Βl������
		if (ratio1 != nullptr)			*ratio1			= RATIO_1;								//����1�̓�����
		if (ratio2 != nullptr)			*ratio2			= RATIO_2;								//����2�̓�����
		if (intersection != nullptr)	*intersection	= startPosition1 + vector1 * RATIO_1;	//��_�̈ʒu

		return true;
	}

	//�s��
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