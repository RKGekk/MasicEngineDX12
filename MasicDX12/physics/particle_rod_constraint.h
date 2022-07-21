#pragma once

#include <DirectXMath.h>

#include "particle_contact.h"
#include "particle_constraint.h"

class ParticleRodConstraint : public ParticleConstraint {
public:
    float length;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override;
};