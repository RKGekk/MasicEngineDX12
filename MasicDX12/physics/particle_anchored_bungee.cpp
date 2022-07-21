#include "particle_anchored_bungee.h"

void ParticleAnchoredBungee::updateForce(Particle* particle, float duration) {
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
