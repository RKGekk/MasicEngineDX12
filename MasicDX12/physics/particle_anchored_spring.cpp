#include "particle_anchored_spring.h"

ParticleAnchoredSpring::ParticleAnchoredSpring() {}

ParticleAnchoredSpring::ParticleAnchoredSpring(DirectX::XMFLOAT3* anchor, float springConstant, float restLength) : m_anchor(anchor), m_springConstant(springConstant), m_restLength(restLength) {}

void ParticleAnchoredSpring::init(DirectX::XMFLOAT3* anchor, float springConstant, float restLength) {
	m_anchor = anchor;
	m_springConstant = springConstant;
	m_restLength = restLength;
}

void ParticleAnchoredSpring::updateForce(Particle* particle, float duration) {
    using namespace DirectX;

    XMVECTOR force = particle->getPosition();
    force -= XMLoadFloat3(m_anchor);

    float magnitude = XMVectorGetX(XMVector3Length(force));
    if (magnitude < m_restLength) { return; }

    magnitude = magnitude - m_restLength;
    magnitude *= m_springConstant;

    force = XMVector3Normalize(force);
    force *= -magnitude;
    particle->addForce(force);
}

const DirectX::XMFLOAT3* ParticleAnchoredSpring::getAnchor() const {
	return m_anchor;
}
