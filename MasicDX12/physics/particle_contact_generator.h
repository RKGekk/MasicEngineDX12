#pragma once

#include "particle_contact.h"

class ParticleContactGenerator {
public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;
};