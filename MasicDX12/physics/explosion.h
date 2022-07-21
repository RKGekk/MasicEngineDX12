#pragma once

#include <DirectXMath.h>

#include "force_generator.h"
#include "particle_force_generator.h"

class Explosion : public ForceGenerator, public ParticleForceGenerator {
    float m_timePassed;

public:
    DirectX::XMFLOAT3 detonation;
    float implosionMaxRadius;
    float implosionMinRadius;
    float implosionDuration;
    float implosionForce;
    float shockwaveSpeed;
    float shockwaveThickness;
    float peakConcussionForce;
    float concussionDuration;
    float peakConvectionForce;
    float chimneyRadius;
    float chimneyHeight;
    float convectionDuration;

    Explosion();

    virtual void updateForce(RigidBody* body, float duration) override;
    virtual void updateForce(Particle* particle, float duration) override = 0;
};