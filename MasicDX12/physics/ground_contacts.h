#pragma once

#include <vector>

#include "particle_contact.h"
#include "particle_contact_generator.h"
#include "particle_world.h"

#include "../engine/i_engine_physics.h"

class GroundContacts : public ParticleContactGenerator {
    float m_ground_level;
    float m_restitution;
    IEnginePhysics* m_physics;

public:
    GroundContacts(float ground_level, float restitution);

    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
};