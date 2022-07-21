#include "contact_resolver.h"

ContactResolver::ContactResolver(unsigned iterations, float velocityEpsilon, float positionEpsilon) {
	setIterations(iterations, iterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

ContactResolver::ContactResolver(unsigned velocityIterations, unsigned positionIterations, float velocityEpsilon, float positionEpsilon) {
	setIterations(velocityIterations);
	setEpsilon(velocityEpsilon, positionEpsilon);
}

bool ContactResolver::isValid() {
	return (m_velocityIterations > 0) && (m_positionIterations > 0) && (m_positionEpsilon >= 0.0f);
}

void ContactResolver::setIterations(unsigned velocityIterations, unsigned positionIterations) {
	m_velocityIterations = velocityIterations;
	m_positionIterations = positionIterations;
}

void ContactResolver::setIterations(unsigned iterations) {
	setIterations(iterations, iterations);
}

void ContactResolver::setEpsilon(float velocityEpsilon, float positionEpsilon) {
	m_velocityEpsilon = velocityEpsilon;
	m_positionEpsilon = positionEpsilon;
}

void ContactResolver::resolveContacts(Contact* contactArray, unsigned numContacts, float duration) {
	if (numContacts == 0) { return;	}
	if (!isValid()) { return; }

	prepareContacts(contactArray, numContacts, duration);
	adjustPositions(contactArray, numContacts, duration);
	adjustVelocities(contactArray, numContacts, duration);
}

void ContactResolver::prepareContacts(Contact* contactArray, unsigned numContacts, float duration) {
	Contact* lastContact = contactArray + numContacts;
	for (Contact* contact = contactArray; contact < lastContact; contact++)     {
		contact->calculateInternals(duration);
	}
}

void ContactResolver::adjustVelocities(Contact* contactArray, unsigned numContacts, float duration) {
    using namespace DirectX;

    XMFLOAT3 velocityChange[2];
    XMFLOAT3 rotationChange[2];
    XMFLOAT3 deltaVel;

    velocityIterationsUsed = 0;
    while (velocityIterationsUsed < m_velocityIterations) {
        float max = m_velocityEpsilon;
        unsigned index = numContacts;
        for (unsigned i = 0; i < numContacts; i++) {
            if (contactArray[i].m_desiredDeltaVelocity > max) {
                max = contactArray[i].m_desiredDeltaVelocity;
                index = i;
            }
        }
        if (index == numContacts) break;

        contactArray[index].matchAwakeState();
        contactArray[index].applyVelocityChange(velocityChange, rotationChange);

        for (unsigned i = 0; i < numContacts; i++) {
            for (unsigned b = 0; b < 2; b++) {
                if (contactArray[i].body[b]) {
                    for (unsigned d = 0; d < 2; d++) {
                        if (contactArray[i].body[b] == contactArray[index].body[d]) {
                            XMStoreFloat3(&deltaVel, XMLoadFloat3(&velocityChange[d]) + XMVector3Cross(XMLoadFloat3(&rotationChange[d]), XMLoadFloat3(&contactArray[i].m_relativeContactPosition[b])));
                            XMStoreFloat3(&contactArray[i].m_contactVelocity, XMLoadFloat3(&contactArray[i].m_contactVelocity) + XMVector3TransformNormal(XMLoadFloat3(&deltaVel), XMLoadFloat3x3(&contactArray[i].m_contactToWorld)) * (b ? -1.0f : 1.0f));
                            contactArray[i].calculateDesiredDeltaVelocity(duration);
                        }
                    }
                }
            }
        }
        velocityIterationsUsed++;
    }
}

void ContactResolver::adjustPositions(Contact* contacts, unsigned numContacts, float duration) {
    using namespace DirectX;

    unsigned i;
    unsigned index;
    XMFLOAT3 linearChange[2];
    XMFLOAT3 angularChange[2];
    float max;
    XMFLOAT3 deltaPosition;

    positionIterationsUsed = 0;
    while (positionIterationsUsed < m_positionIterations) {
        max = m_positionEpsilon;
        index = numContacts;
        for (i = 0; i < numContacts; i++) {
            if (contacts[i].penetration > max) {
                max = contacts[i].penetration;
                index = i;
            }
        }
        if (index == numContacts) { break; }

        contacts[index].matchAwakeState();
        contacts[index].applyPositionChange(linearChange, angularChange, max);

        for (i = 0; i < numContacts; i++) {
            for (unsigned b = 0; b < 2; b++) {
                if (contacts[i].body[b]) {
                    for (unsigned d = 0; d < 2; d++) {
                        if (contacts[i].body[b] == contacts[index].body[d]) {
                            XMStoreFloat3(&deltaPosition, XMLoadFloat3(&linearChange[d]) + XMVector3Cross(XMLoadFloat3(&angularChange[d]), XMLoadFloat3(&contacts[i].m_relativeContactPosition[b])));
                            contacts[i].penetration += XMVectorGetX(XMVector3Dot(XMLoadFloat3(&deltaPosition), XMLoadFloat3(&contacts[i].contactNormal))) * (b ? 1.0f : -1.0f);
                        }
                    }
                }
            }
        }
        positionIterationsUsed++;
    }
}
