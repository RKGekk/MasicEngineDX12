#include "particle_cable.h"

unsigned ParticleCable::addContact(ParticleContact* contact, unsigned limit) const {
    using namespace DirectX;

    float length = currentLength();
    if (length < maxLength) { return 0; }

    contact->particle[0] = particle[0];
    contact->particle[1] = particle[1];

    XMVECTOR normal = XMVector3Normalize(particle[1]->getPosition() - particle[0]->getPosition());
    XMStoreFloat3(&contact->contactNormal, normal);

    contact->penetration = length - maxLength;
    contact->restitution = restitution;

    return 1;
}
