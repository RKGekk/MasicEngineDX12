#pragma once

#include <DirectXMath.h>

#include "particle_contact.h"
#include "particle_link.h"

class ParticleRod : public ParticleLink {
public:
    float length;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override;
};