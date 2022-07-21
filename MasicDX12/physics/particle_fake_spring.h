#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_force_generator.h"

class ParticleFakeSpring : public ParticleForceGenerator {
    DirectX::XMFLOAT3* m_anchor;
    float m_springConstant;
    float m_damping;

public:
    ParticleFakeSpring(DirectX::XMFLOAT3* anchor, float springConstant, float damping);

    virtual void updateForce(Particle* particle, float duration) override;
};