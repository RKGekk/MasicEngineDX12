#include "particle_constraint.h"

float ParticleConstraint::currentLength() const {
	using namespace DirectX;

	XMVECTOR relativePos = particle->getPosition() - XMLoadFloat3(&anchor);
	return XMVectorGetX(XMVector3Length(relativePos));
}
