#include "particle_sphere_contact.h"
#include "../engine/engine.h"

void ParticleSphereContact::init(ParticleWorld::Particles* particles) {
    m_particles = particles;
}

unsigned ParticleSphereContact::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;

    unsigned count = 0;
    //for (ParticleWorld::Particles::iterator p1 = m_particles->begin(); p1 != m_particles->end(); ++p1) {
    //    for (ParticleWorld::Particles::iterator p2 = m_particles->begin(); p2 != m_particles->end(); ++p2) {
    //        if (p2 != p1) {
    //            XMVECTOR particle1Pos = (*p1)->getPosition();
    //            XMVECTOR particle2Pos = (*p2)->getPosition();
    //            float r1 = (*p1)->getRadius();
    //            float r2 = (*p2)->getRadius();
    //
    //            XMVECTOR contactTrace = particle1Pos - particle2Pos;
    //            float distance = XMVectorGetX(XMVector3Length(contactTrace));
    //
    //            if (distance < (r1 + r2)) {
    //                XMStoreFloat3(&contact->contactNormal, XMVector3Normalize(contactTrace));
    //                contact->particle[0] = *p1;
    //                contact->particle[1] = *p2;
    //                contact->penetration = (r1 + r2) - distance;
    //                //contact->restitution = 1.0f;
    //                contact->restitution = 0.2f;
    //                contact++;
    //                count++;
    //            }
    //
    //            if (count >= limit) { return count; }
    //        }
    //    }
    //}
    
    size_t num_particles = m_particles->size();
    if(num_particles < 2u) { return count; }
    for (size_t i = 0; i < num_particles - 1; ++i) {
        for (size_t j = i + 1; j < num_particles; ++j) {
            Particle* p1 = (*m_particles)[i];
            Particle* p2 = (*m_particles)[j];
            XMVECTOR particle1Pos = p1->getPosition();
            XMVECTOR particle2Pos = p2->getPosition();
            float r1 = p1->getRadius();
            float r2 = p2->getRadius();

            XMVECTOR contactTrace = particle1Pos - particle2Pos;
            float distance = XMVectorGetX(XMVector3Length(contactTrace));

            if (distance < (r1 + r2)) {
                XMStoreFloat3(&contact->contactNormal, XMVector3Normalize(contactTrace));
                contact->particle[0] = p1;
                contact->particle[1] = p2;
                contact->penetration = (r1 + r2) - distance;
                //contact->restitution = 1.0f;
                contact->restitution = 0.2f;
                contact++;
                count++;
            }

            if (count >= limit) { return count; }
        }
    }
    return count;
}
