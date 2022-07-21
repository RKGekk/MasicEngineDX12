#pragma once

#include <DirectXMath.h>

#include "particle_link.h"

class ParticleCable : public ParticleLink {
public:
    float maxLength;
    float restitution;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override;
};