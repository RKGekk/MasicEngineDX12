#pragma once

#include <DirectXMath.h>

#include "contact.h"

class ContactResolver {
protected:
    unsigned m_velocityIterations;
    unsigned m_positionIterations;
    float m_velocityEpsilon;
    float m_positionEpsilon;
    bool m_validSettings;

public:
    unsigned velocityIterationsUsed;
    unsigned positionIterationsUsed;

    ContactResolver(unsigned iterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);
    ContactResolver(unsigned velocityIterations, unsigned positionIterations, float velocityEpsilon = 0.01f, float positionEpsilon = 0.01f);

    bool isValid();
    void setIterations(unsigned velocityIterations, unsigned positionIterations);
    void setIterations(unsigned iterations);
    void setEpsilon(float velocityEpsilon, float positionEpsilon);
    void resolveContacts(Contact* contactArray, unsigned numContacts, float duration);

protected:
    void prepareContacts(Contact* contactArray, unsigned numContacts, float duration);
    void adjustVelocities(Contact* contactArray, unsigned numContacts, float duration);
    void adjustPositions(Contact* contacts, unsigned numContacts, float duration);
};