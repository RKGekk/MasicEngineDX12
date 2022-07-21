#pragma once

#include <DirectXMath.h>
#include <cmath>

#include "particle.h"
#include "particle_force_generator.h"

class ParticleSpring : public ParticleForceGenerator     {
    Particle* m_other;
    float m_springConstant;
    float m_restLength;

public:
    ParticleSpring(Particle* other, float springConstant, float restLength);

    virtual void updateForce(Particle* particle, float duration) override;
};