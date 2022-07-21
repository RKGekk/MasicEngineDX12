#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_anchored_spring.h"

class ParticleAnchoredBungee : public ParticleAnchoredSpring {
public:
    virtual void updateForce(Particle* particle, float duration) override;
};