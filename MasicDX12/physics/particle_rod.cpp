#include "particle_rod.h"
#include "../tools/math_utitity.h"

unsigned ParticleRod::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;

    float currentLen = currentLength();
    if (XMScalarNearEqual(currentLen, length, EPSILON)) { return 0; }

    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    XMVECTOR normal = XMVector3Normalize(particle[1]->getPosition() - particle[0]->getPosition());

    if (currentLen > length) {
        XMStoreFloat3(&contact->contactNormal, normal);
        contact->penetration = currentLen - length;
    }
    else {
        XMStoreFloat3(&contact->contactNormal, normal * -1.0f);
        contact->penetration = length - currentLen;
    }

    contact->restitution = 0;

    return 1;
}
