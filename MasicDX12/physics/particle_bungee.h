#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_force_generator.h"

class ParticleBungee : public ParticleForceGenerator {
    Particle* m_other;
    float m_springConstant;
    float m_restLength;

public:
    ParticleBungee(Particle* other, float springConstant, float restLength);

    virtual void updateForce(Particle* particle, float duration);
};