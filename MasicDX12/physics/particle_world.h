#pragma once

#include <vector>

#include "particle.h"
#include "particle_contact.h"
#include "particle_contact_resolver.h"
#include "particle_contact_generator.h"
#include "particle_force_registry.h"

class ParticleWorld {
public:
    typedef std::vector<Particle*> Particles;
    typedef std::vector<ParticleContactGenerator*> ContactGenerators;
    typedef std::vector<ParticleContact> ParticleContacts;

protected:
    Particles m_particles;
    bool m_calculateIterations;
    ParticleForceRegistry m_registry;
    ParticleContactResolver m_resolver;
    ContactGenerators m_contactGenerators;
    ParticleContacts m_contacts;
    unsigned m_maxContacts;

public:
    ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
    ~ParticleWorld();

    unsigned generateContacts();
    void integrate(float duration);
    void runPhysics(float duration);
    void startFrame();

    Particles& getParticles();
    Particles* getParticlesPtr();
    ContactGenerators& getContactGenerators();
    ContactGenerators* getContactGeneratorsPtr();
    ParticleForceRegistry& getForceRegistry();
    ParticleForceRegistry* getForceRegistryPtr();
};