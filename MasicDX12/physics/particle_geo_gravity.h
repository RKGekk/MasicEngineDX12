#pragma once

#include <DirectXMath.h>

#include "particle_force_generator.h"

class ParticleGeoGravity : public ParticleForceGenerator {
private:
    DirectX::XMFLOAT3 m_center;
    float m_g;

public:
    ParticleGeoGravity(const DirectX::XMFLOAT3& center, float g);
    ParticleGeoGravity(DirectX::FXMVECTOR center, float g);

    virtual void updateForce(Particle* particle, float duration) override;
};