#include "particle_bungee.h"

ParticleBungee::ParticleBungee(Particle* other, float springConstant, float restLength) : m_other(other), m_springConstant(springConstant), m_restLength(restLength) {}

void ParticleBungee::updateForce(Particle* particle, float duration) {
    using namespace DirectX;
    
    XMVECTOR force = particle->getPosition();
    force -= m_other->getPosition();

    float magnitude = XMVectorGetX(XMVector3Length(force));
    if (magnitude <= m_restLength) { return; }

    magnitude = m_springConstant * (m_restLength - magnitude);

    force = XMVector3Normalize(force);
    force *= -magnitude;
    particle->addForce(force);
}