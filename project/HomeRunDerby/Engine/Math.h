#pragma once
#include "Transform.h"

namespace Math
{
	using DirectX::XMVECTOR;

	//�O�p�`�Ɛ����̌�������
	//�����P:origin			���C�̎n�_
	//�����Q:ray			���C�̌����x�N�g��
	//�����R:vertex0		���肷��O�p�̒��_
	//�����S:vertex1		���肷��O�p�̒��_
	//�����T:vertex2		���肷��O�p�̒��_
	//�����U:distance		���C�̎n�_�����_�܂ł̋���(�o��)
	//�ߒl:�������Ă��邩
	bool IntersectLineToPolygon(const XMVECTOR& origin, const XMVECTOR& ray,
		const XMVECTOR& vertex0, const XMVECTOR& vertex1, const XMVECTOR& vertex2, float* distance);


	//�񎟌���̐������m�̌�������
	//�����P:startPosition1	����1�̎n�_
	//�����Q:startPosition2	����2�̎n�_
	//�����R:vector1		����1�̃x�N�g��
	//�����S:vector2		����2�̃x�N�g��
	//�����T:ratio1			����1�̓�����(�o��)
	//�����U:ratio2			����2�̓�����(�o��)
	//�����V:intersection	��_(�o��)
	//�ߒl:�������Ă��邩
	bool IntersectLineToLine2D(const XMVECTOR&	startPosition1, const XMVECTOR&	startPosition2,
		const XMVECTOR&	vector1, const XMVECTOR& vector2, float* ratio1, float* ratio2, XMVECTOR* intersection);

	//�s��	
	//����:edge1			�x�N�g��1
	//����:edge2			�x�N�g��2
	//����:edge3			�x�N�g��3
	//�ߒl:�g��{��
	float Det(const XMVECTOR& edge1, const XMVECTOR& edge2, const XMVECTOR& edge3);
}
