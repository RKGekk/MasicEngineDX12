#pragma once

#include <DirectXMath.h>

#include "particle_force_generator.h"

class ParticleAnchoredSpring : public ParticleForceGenerator {
protected:
    DirectX::XMFLOAT3* m_anchor;
    float m_springConstant;
    float m_restLength;

public:
    ParticleAnchoredSpring();
    ParticleAnchoredSpring(DirectX::XMFLOAT3* anchor, float springConstant, float restLength);

    void init(DirectX::XMFLOAT3* anchor, float springConstant, float restLength);
    virtual void updateForce(Particle* particle, float duration) override;

    const DirectX::XMFLOAT3* getAnchor() const;
};