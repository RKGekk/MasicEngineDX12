#include "particle_drag.h"

ParticleDrag::ParticleDrag(float k1, float k2) : m_k1(k1), m_k2(k2) {}

void ParticleDrag::updateForce(Particle* particle, float duration) {
    using namespace DirectX;

    XMVECTOR force = particle->getVelocity();

    float dragCoeff = XMVectorGetX(XMVector3Length(force));
    dragCoeff = m_k1 * dragCoeff + m_k2 * dragCoeff * dragCoeff;

    force = DirectX::XMVector3Normalize(force);
    force *= -dragCoeff;
    particle->addForce(force);
}
