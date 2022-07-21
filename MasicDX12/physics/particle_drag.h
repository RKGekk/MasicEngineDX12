#pragma once

#include <DirectXMath.h>

#include "particle_force_generator.h"

class ParticleDrag : public ParticleForceGenerator {
private:
    float m_k1;
    float m_k2;

public:
    ParticleDrag(float k1, float k2);

    virtual void updateForce(Particle* particle, float duration) override;
};