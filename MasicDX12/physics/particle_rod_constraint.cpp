#include "particle_rod_constraint.h"
#include "../tools/math_utitity.h"

unsigned ParticleRodConstraint::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;
	
    float currentLen = currentLength();
    if (XMScalarNearEqual(currentLen, length, EPSILON)) { return 0; }

    contact->particle[0] = particle;
    contact->particle[1] = nullptr;

    XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&anchor) - particle->getPosition());

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