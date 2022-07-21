#pragma once

#include <DirectXMath.h>

#include "particle_contact.h"
#include "particle_constraint.h"

class ParticleCableConstraint : public ParticleConstraint {
public:
    float maxLength;
    float restitution;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override;
};