#include "ground_contacts.h"
#include "../tools/math_utitity.h"
#include "../engine/engine.h"

GroundContacts::GroundContacts(float ground_level, float restitution) : m_ground_level(ground_level), m_restitution(restitution) {
    m_physics = g_pApp->GetGameLogic()->VGetGamePhysics();
}

unsigned GroundContacts::addContact(ParticleContact* contact, unsigned limit) const {
    unsigned count = 0;
    ParticleWorld::Particles& particles = m_physics->VGetParticles();
    for (ParticleWorld::Particles::iterator p = particles.begin(); p != particles.end(); p++) {
        float y = (*p)->getPosition3f().y;
        float r = (*p)->getRadius();
        if (y < (m_ground_level + r)) {
            contact->contactNormal = DEFAULT_UP_VECTOR;
            contact->particle[0] = *p;
            contact->particle[1] = nullptr;
            contact->penetration = m_ground_level + r - y;
            contact->restitution = m_restitution;
            contact++;
            count++;
        }

        if (count >= limit) {
            return count;
        }
    }
    return count;
}
