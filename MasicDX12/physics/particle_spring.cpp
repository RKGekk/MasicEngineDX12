#include "particle_spring.h"

ParticleSpring::ParticleSpring(Particle* other, float springConstant, float restLength) : m_other(other), m_springConstant(springConstant), m_restLength(restLength) {}

void ParticleSpring::updateForce(Particle* particle, float duration) {
    using namespace DirectX;

    XMVECTOR force = particle->getPosition();
    force -= m_other->getPosition();

    float magnitude = XMVectorGetX(XMVector3Length(force));
    magnitude = std::abs(magnitude - m_restLength);
    magnitude *= m_springConstant;

    force = XMVector3Normalize(force);
    force *= -magnitude;
    particle->addForce(force);
}
