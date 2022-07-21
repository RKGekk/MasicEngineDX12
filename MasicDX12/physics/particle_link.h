#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_contact.h"
#include "particle_contact_generator.h"

class ParticleLink : public ParticleContactGenerator {
public:
    Particle* particle[2];

protected:
    float currentLength() const;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override = 0;
};