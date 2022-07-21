#include "particle_cable_constraint.h"

unsigned ParticleCableConstraint::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;

    float length = currentLength();
    if (length < maxLength) { return 0; }

    contact->particle[0] = particle;
    contact->particle[1] = 0;

    XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&anchor) - particle->getPosition());
    XMStoreFloat3(&contact->contactNormal, normal);

    contact->penetration = length - maxLength;
    contact->restitution = restitution;

    return 1;
}
