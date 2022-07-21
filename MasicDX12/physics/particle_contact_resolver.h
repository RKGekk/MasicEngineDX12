#pragma once

#include <limits>

#include <DirectXMath.h>

#include "particle_contact.h"

class ParticleContactResolver {
protected:
    unsigned m_iterations;
    unsigned m_iterationsUsed;

public:
    ParticleContactResolver(unsigned iterations);

    void setIterations(unsigned iterations);
    void resolveContacts(ParticleContact* contactArray, unsigned numContacts, float duration);
};