#include "particle_buoyancy.h"

ParticleBuoyancy::ParticleBuoyancy(float maxDepth, float volume, float waterHeight, float liquidDensity) : m_maxDepth(maxDepth), m_volume(volume), m_waterHeight(waterHeight), m_liquidDensity(liquidDensity) {}

void ParticleBuoyancy::updateForce(Particle* particle, float duration) {
    using namespace DirectX;

    float depth = particle->getPosition3f().y;

    if (depth >= m_waterHeight + m_maxDepth) { return; }
    XMVECTOR force = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    if (depth <= m_waterHeight - m_maxDepth)     {
        force = XMVectorSetY(force, m_liquidDensity * m_volume);
        particle->addForce(force);
        return;
    }

    force = XMVectorSetY(force, m_liquidDensity * m_volume * (depth - m_maxDepth - m_waterHeight) / (2.0f * m_maxDepth));
    particle->addForce(force);
}