#include "particle_world.h"

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations) : m_resolver(iterations), m_maxContacts(maxContacts) {
	m_contacts.reserve(maxContacts);
	m_calculateIterations = (iterations == 0);
}

ParticleWorld::~ParticleWorld() {}

unsigned ParticleWorld::generateContacts() {
    unsigned limit = m_maxContacts;
    ParticleContact* nextContact = m_contacts.data();

    for (ContactGenerators::iterator g = m_contactGenerators.begin(); g != m_contactGenerators.end(); g++) {
        unsigned used = (*g)->addContact(nextContact, limit);
        limit -= used;
        nextContact += used;
        if (limit <= 0) { break; }
    }

    return m_maxContacts - limit;
}

void ParticleWorld::integrate(float duration) {
    for (Particles::iterator p = m_particles.begin(); p != m_particles.end(); p++) {
        (*p)->integrate(duration);
    }
}

void ParticleWorld::runPhysics(float duration) {
    m_registry.updateForces(duration);
    integrate(duration);

    unsigned usedContacts = generateContacts();

    if (usedContacts) {
        if (m_calculateIterations) {
            m_resolver.setIterations(usedContacts * 2);
        }
        m_resolver.resolveContacts(m_contacts.data(), usedContacts, duration);
    }
}

void ParticleWorld::startFrame() {
    for (Particles::iterator p = m_particles.begin(); p != m_particles.end(); p++) {
        (*p)->clearAccumulator();
    }
}

ParticleWorld::Particles& ParticleWorld::getParticles() {
    return m_particles;
}

ParticleWorld::Particles* ParticleWorld::getParticlesPtr() {
    return &m_particles;
}

ParticleWorld::ContactGenerators& ParticleWorld::getContactGenerators() {
    return m_contactGenerators;
}

ParticleWorld::ContactGenerators* ParticleWorld::getContactGeneratorsPtr() {
    return &m_contactGenerators;
}

ParticleForceRegistry& ParticleWorld::getForceRegistry() {
    return m_registry;
}

ParticleForceRegistry* ParticleWorld::getForceRegistryPtr() {
    return &m_registry;
}
