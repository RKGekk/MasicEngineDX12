#pragma once

#include <DirectXMath.h>

#include "particle.h"

class ParticleContactResolver;

class ParticleContact {
    friend class ParticleContactResolver;

public:
    Particle* particle[2];

    float restitution;
    float penetration;
    DirectX::XMFLOAT3 contactNormal;
    DirectX::XMFLOAT3 particleMovement[2];

protected:
    void resolve(float duration);
    float calculateSeparatingVelocity() const;

private:
    void resolveVelocity(float duration);
    void resolveInterpenetration(float duration);
    void matchAwakeState();
};