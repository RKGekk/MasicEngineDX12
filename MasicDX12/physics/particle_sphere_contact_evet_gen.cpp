#include "particle_sphere_contact_evet_gen.h"
#include "../events/evt_data_sphere_particle_contact.h"
#include "../events/i_event_manager.h"
#include "../engine/engine.h"

void ParticleSphereContactEventGen::init(ParticleWorld::Particles* particles) {
	m_particles = particles;
}

unsigned ParticleSphereContactEventGen::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;
    unsigned count = 0;
    size_t num_particles = m_particles->size();
    if (num_particles < 2u) { return 0; }
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
                IEnginePhysics* engine_phys = g_pApp->GetGameLogic()->VGetGamePhysics();
                ActorId act1 = engine_phys->VGetParticleActor(p1);
                ActorId act2 = engine_phys->VGetParticleActor(p2);
                std::shared_ptr<EvtData_Sphere_Particle_Contact> pNewActorEvent(new EvtData_Sphere_Particle_Contact(act1, act2));
                IEventManager::Get()->VQueueEvent(pNewActorEvent);

                //XMStoreFloat3(&contact->contactNormal, XMVector3Normalize(contactTrace));
                //contact->particle[0] = p1;
                //contact->particle[1] = p2;
                //contact->penetration = (r1 + r2) - distance;
                ////contact->restitution = 1.0f;
                //contact->restitution = 0.2f;
                //contact++;

                count++;
            }

            if (count >= limit) { return count; }
        }
    }

	return 0;
}
