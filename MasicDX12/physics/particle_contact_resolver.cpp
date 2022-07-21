#include "particle_contact_resolver.h"

ParticleContactResolver::ParticleContactResolver(unsigned iterations) : m_iterations(iterations), m_iterationsUsed(0) {}

void ParticleContactResolver::setIterations(unsigned iterations) {
	m_iterations = iterations;
}

void ParticleContactResolver::resolveContacts(ParticleContact* contactArray, unsigned numContacts, float duration) {
    using namespace DirectX;

    unsigned i;
    m_iterationsUsed = 0;
    while (m_iterationsUsed < m_iterations) {
        float max = std::numeric_limits<float>::max();
        unsigned maxIndex = numContacts;
        for (i = 0; i < numContacts; i++) {
            contactArray[i].matchAwakeState();
            float sepVel = contactArray[i].calculateSeparatingVelocity();
            if (sepVel < max && (sepVel < 0 || contactArray[i].penetration > 0)) {
                max = sepVel;
                maxIndex = i;
            }
        }

        if (maxIndex == numContacts) { break; }

        contactArray[maxIndex].resolve(duration);

        XMVECTOR move0 = XMLoadFloat3(&contactArray[maxIndex].particleMovement[0]);
        XMVECTOR move1 = XMLoadFloat3(&contactArray[maxIndex].particleMovement[1]);
        for (i = 0; i < numContacts; i++) {
            if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0]) {
                contactArray[i].penetration -= XMVectorGetX(XMVector3Dot(move0, XMLoadFloat3(&contactArray[i].contactNormal)));
            }
            else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1]) {
                contactArray[i].penetration -= XMVectorGetX(XMVector3Dot(move1, XMLoadFloat3(&contactArray[i].contactNormal)));
            }

            if (contactArray[i].particle[1]) {
                if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0]) {
                    contactArray[i].penetration += XMVectorGetX(XMVector3Dot(move0, XMLoadFloat3(&contactArray[i].contactNormal)));
                }
                else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1]) {
                    contactArray[i].penetration += XMVectorGetX(XMVector3Dot(move1, XMLoadFloat3(&contactArray[i].contactNormal)));
                }
            }
        }

        m_iterationsUsed++;
    }
}