#pragma once

#include <DirectXMath.h>

#include "particle.h"
#include "particle_contact.h"
#include "particle_contact_generator.h"

class ParticleConstraint : public ParticleContactGenerator {
public:
    Particle* particle;
    DirectX::XMFLOAT3 anchor;

protected:
    float currentLength() const;

public:
    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override = 0;
};