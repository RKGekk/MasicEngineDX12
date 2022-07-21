#pragma once

#include <DirectXMath.h>

#include "particle_force_generator.h"

class ParticleGravity : public ParticleForceGenerator {
private:
    DirectX::XMFLOAT3 m_gravity;

public:
    ParticleGravity(const DirectX::XMFLOAT3& gravity);
    ParticleGravity(DirectX::FXMVECTOR gravity);

    virtual void updateForce(Particle* particle, float duration) override;
};