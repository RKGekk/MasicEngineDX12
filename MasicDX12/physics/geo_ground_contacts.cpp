#include "geo_ground_contacts.h"
#include "../tools/math_utitity.h"
#include "../engine/engine.h"

GeoGroundContacts::GeoGroundContacts(const DirectX::XMFLOAT3& center, float ground_level, float restitution) : m_ground_level(ground_level), m_restitution(restitution), m_center(center) {
    m_physics = g_pApp->GetGameLogic()->VGetGamePhysics();
}

GeoGroundContacts::GeoGroundContacts(DirectX::XMVECTOR center, float ground_level, float restitution) : m_ground_level(ground_level), m_restitution(restitution) {
    DirectX::XMStoreFloat3(&m_center, center);
    m_physics = g_pApp->GetGameLogic()->VGetGamePhysics();
}

unsigned GeoGroundContacts::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;

    unsigned count = 0;
    ParticleWorld::Particles& particles = m_physics->VGetParticles();
    for (ParticleWorld::Particles::iterator p = particles.begin(); p != particles.end(); p++) {
        XMVECTOR pos = (*p)->getPosition();
        XMVECTOR center = XMLoadFloat3(&m_center);
        XMVECTOR dir = pos - center;
        XMVECTOR normal = XMVector3Normalize(dir);
        float dist = XMVectorGetX(XMVector3Length(dir));
        float r = (*p)->getRadius();

        if (dist < (m_ground_level + r)) {
            XMStoreFloat3(&contact->contactNormal, normal);
            contact->particle[0] = *p;
            contact->particle[1] = nullptr;
            contact->penetration = m_ground_level + r - dist;
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
