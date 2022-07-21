#include "particle_force_registry.h"
#include <algorithm>

void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* fg) {
    m_registrations.push_back({ particle, fg });
}

void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* fg) {
    ParticleForceRegistration tempFR = { particle, fg };
    m_registrations.erase(std::find(m_registrations.begin(), m_registrations.end(), tempFR));
}

void ParticleForceRegistry::clear() {
    m_registrations.clear();
}

void ParticleForceRegistry::updateForces(float duration) {
    Registry::iterator i = m_registrations.begin();
    for (; i != m_registrations.end(); i++) {
        i->fg->updateForce(i->particle, duration);
    }
}
