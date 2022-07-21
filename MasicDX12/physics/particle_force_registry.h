#pragma once

#include <vector>

#include "particle.h"
#include "particle_force_generator.h"

class ParticleForceRegistry {
protected:

    struct ParticleForceRegistration {
        Particle* particle;
        ParticleForceGenerator* fg;
        bool operator==(const ParticleForceRegistration& right) {
            return this->particle == right.particle && this->fg == right.fg;
        };
    };

    typedef std::vector<ParticleForceRegistration> Registry;
    Registry m_registrations;

public:
    void add(Particle* particle, ParticleForceGenerator* fg);
    void remove(Particle* particle, ParticleForceGenerator* fg);
    void clear();
    void updateForces(float duration);
};