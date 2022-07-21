#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_force_generator.h"

class ParticleBuoyancy : public ParticleForceGenerator     {
    float m_maxDepth;
    float m_volume;
    float m_waterHeight;
    float m_liquidDensity;

public:
    ParticleBuoyancy(float maxDepth, float volume, float waterHeight, float liquidDensity = 1000.0f);

    virtual void updateForce(Particle* particle, float duration);
};