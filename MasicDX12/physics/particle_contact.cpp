#include "particle_contact.h"

void ParticleContact::resolve(float duration) {
	resolveVelocity(duration);
	resolveInterpenetration(duration);
}

float ParticleContact::calculateSeparatingVelocity() const {
	using namespace DirectX;

	XMVECTOR relativeVelocity = particle[0]->getVelocity();
	if (particle[1]) {
		relativeVelocity -= particle[1]->getVelocity();
	}
	return XMVectorGetX(XMVector3Dot(relativeVelocity, XMLoadFloat3(&contactNormal)));
}

void ParticleContact::resolveVelocity(float duration) {
    using namespace DirectX;

    float separatingVelocity = calculateSeparatingVelocity();
    if (separatingVelocity > 0) { return; }

    float newSepVelocity = -separatingVelocity * restitution;

    XMVECTOR accCausedVelocity = particle[0]->getAcceleration();
    if (particle[1]) {
        accCausedVelocity -= particle[1]->getAcceleration();
    }
    float accCausedSepVelocity = XMVectorGetX(XMVector3Dot(accCausedVelocity, XMLoadFloat3(&contactNormal))) * duration;

    if (accCausedSepVelocity < 0) {
        newSepVelocity += restitution * accCausedSepVelocity;
        if (newSepVelocity < 0) {
            newSepVelocity = 0;
        }
    }

    float deltaVelocity = newSepVelocity - separatingVelocity;

    float totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) {
        totalInverseMass += particle[1]->getInverseMass();
    }

    if (totalInverseMass <= 0.0f) { return; }

    float impulse = deltaVelocity / totalInverseMass;
    XMVECTOR impulsePerIMass = XMLoadFloat3(&contactNormal) * impulse;
    particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());
    if (particle[1]) {
        particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
    }
}

void ParticleContact::resolveInterpenetration(float duration) {
    using namespace DirectX;
    if (penetration <= 0) { return; }

    float totalInverseMass = particle[0]->getInverseMass();
    if (particle[1]) {
        totalInverseMass += particle[1]->getInverseMass();
    }

    if (totalInverseMass <= 0) { return; }

    XMVECTOR movePerIMass = XMLoadFloat3(&contactNormal) * (penetration / totalInverseMass);

    XMVECTOR particleMovement0 = movePerIMass * particle[0]->getInverseMass();
    XMStoreFloat3(&particleMovement[0], particleMovement0);

    XMVECTOR particleMovement1;
    if (particle[1]) {
        particleMovement1 = movePerIMass * -particle[1]->getInverseMass();
        XMStoreFloat3(&particleMovement[1], particleMovement1);
    }
    else {
        particleMovement[1].x = 0.0f;
        particleMovement[1].y = 0.0f;
        particleMovement[1].z = 0.0f;
    }

    particle[0]->setPosition(particle[0]->getPosition() + particleMovement0);
    if (particle[1]) {
        particle[1]->setPosition(particle[1]->getPosition() + particleMovement1);
    }
}

void ParticleContact::matchAwakeState() {
    if (!particle[1]) { return; }

    bool body0awake = particle[0]->getAwake();
    bool body1awake = particle[1]->getAwake();

    if (body0awake ^ body1awake) {
        if (body0awake) {
            particle[1]->setAwake();
        }
        else {
            particle[0]->setAwake();
        }
    }
}

