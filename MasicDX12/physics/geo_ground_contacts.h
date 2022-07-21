#pragma once

#include <vector>

#include "particle_contact.h"
#include "particle_contact_generator.h"
#include "particle_world.h"

#include "../engine/i_engine_physics.h"

class GeoGroundContacts : public ParticleContactGenerator {
    float m_ground_level;
    float m_restitution;
    DirectX::XMFLOAT3 m_center;
    IEnginePhysics* m_physics;

public:
    GeoGroundContacts(const DirectX::XMFLOAT3& center, float ground_level, float restitution);
    GeoGroundContacts(DirectX::XMVECTOR center, float ground_level, float restitution);

    virtual unsigned addContact(ParticleContact* contact, unsigned limit) const;
};