#include "contact.h"
#include <cmath>
#include "../tools/math_utitity.h"

void Contact::applyImpulse(DirectX::FXMVECTOR impulse, RigidBody* body, DirectX::XMFLOAT3* velocityChange, DirectX::XMFLOAT3* rotationChange) {}

void Contact::applyVelocityChange(DirectX::XMFLOAT3 velocityChange[2], DirectX::XMFLOAT3 rotationChange[2]) {
    using namespace DirectX;

    XMMATRIX inverseInertiaTensor1 = body[0]->getInverseInertiaTensorWorld();
    XMMATRIX inverseInertiaTensor2;
    if (body[1]) {
        inverseInertiaTensor2 = body[1]->getInverseInertiaTensorWorld();
    }
    
    XMVECTOR impulseContact;

    if (XMScalarNearEqual(friction, 0.0f, EPSILON)) {
        impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor1, inverseInertiaTensor2);
    }
    else {
        impulseContact = calculateFrictionImpulse(inverseInertiaTensor1, inverseInertiaTensor2);
    }

    XMVECTOR impulse = XMVector3TransformNormal(impulseContact, XMLoadFloat3x3(&m_contactToWorld));

    XMVECTOR impulsiveTorque = XMVector3Cross(XMLoadFloat3(&m_relativeContactPosition[0]), impulse);
    XMStoreFloat3(&rotationChange[0], XMVector3TransformNormal(impulsiveTorque, inverseInertiaTensor1));
    XMStoreFloat3(&velocityChange[0], impulse * body[0]->getInverseMass());

    body[0]->addVelocity3f(velocityChange[0]);
    body[0]->addRotation3f(rotationChange[0]);

    if (body[1]) {
        XMVECTOR impulsiveTorque = XMVector3Cross(impulse, XMLoadFloat3(&m_relativeContactPosition[1]));
        XMStoreFloat3(&rotationChange[1], XMVector3TransformNormal(impulsiveTorque, inverseInertiaTensor2));
        XMStoreFloat3(&velocityChange[1], impulse * -body[1]->getInverseMass());

        body[1]->addVelocity3f(velocityChange[1]);
        body[1]->addRotation3f(rotationChange[1]);
    }
}

void Contact::applyPositionChange(DirectX::XMFLOAT3 linearChange[2], DirectX::XMFLOAT3 angularChange[2], float penetration) {
    using namespace DirectX;

    const float angularLimit = 0.2f;
    float angularMove[2];
    float linearMove[2];

    float totalInertia = 0.0f;
    float linearInertia[2];
    float angularInertia[2];

    XMVECTOR contactNormalXM = XMLoadFloat3(&contactNormal);

    for (unsigned i = 0; i < 2; i++) if (body[i]) {
        XMMATRIX inverseInertiaTensor =  body[i]->getInverseInertiaTensorWorld();
        XMVECTOR relativeContactPositionXM = XMLoadFloat3(&m_relativeContactPosition[i]);

        XMVECTOR angularInertiaWorld = XMVector3Cross(relativeContactPositionXM, contactNormalXM);
        angularInertiaWorld = XMVector3TransformNormal(angularInertiaWorld, inverseInertiaTensor);
        angularInertiaWorld = XMVector3Cross(angularInertiaWorld, relativeContactPositionXM);
        angularInertia[i] = XMVectorGetX(XMVector3Dot(angularInertiaWorld, contactNormalXM));

        linearInertia[i] = body[i]->getInverseMass();
        totalInertia += linearInertia[i] + angularInertia[i];
    }

    for (unsigned i = 0; i < 2; i++) if (body[i]) {
        float sign = (i == 0) ? 1.0f : -1.0f;
        angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
        linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);
        XMVECTOR relativeContactPositionXM = XMLoadFloat3(&m_relativeContactPosition[i]);
        XMVECTOR projection = relativeContactPositionXM;
        projection += contactNormalXM * -XMVectorGetX(XMVector3Dot(relativeContactPositionXM, contactNormalXM));

        float maxMagnitude = angularLimit * XMVectorGetX(XMVector3Length(projection));

        if (angularMove[i] < -maxMagnitude) {
            float totalMove = angularMove[i] + linearMove[i];
            angularMove[i] = -maxMagnitude;
            linearMove[i] = totalMove - angularMove[i];
        }
        else if (angularMove[i] > maxMagnitude) {
            float totalMove = angularMove[i] + linearMove[i];
            angularMove[i] = maxMagnitude;
            linearMove[i] = totalMove - angularMove[i];
        }

        if (XMScalarNearEqual(angularMove[i], 0.0f, EPSILON)) {
            angularChange[i].x = 0.0f;
        }
        else {
            XMVECTOR targetAngularDirection = XMVector3Cross(relativeContactPositionXM, contactNormalXM);
            XMMATRIX inverseInertiaTensor = body[i]->getInverseInertiaTensorWorld();
            XMStoreFloat3(&angularChange[i], XMVector3TransformNormal(targetAngularDirection, inverseInertiaTensor) * (angularMove[i] / angularInertia[i]));
        }

        XMStoreFloat3(&linearChange[i], contactNormalXM * linearMove[i]);

        XMVECTOR pos = body[i]->getPosition();
        pos += contactNormalXM * linearMove[i];
        body[i]->setPosition(pos);

        XMVECTOR q_old = XMLoadFloat4(&body[i]->getOrientation4f());
        XMVECTOR q = q_old + XMQuaternionMultiply(XMVectorSetW(XMLoadFloat3(&angularChange[i]), 0.0f), q_old) * 0.5f;
        body[i]->setOrientation(q);

        if (!body[i]->getAwake()) {
            body[i]->calculateDerivedData();
        }
    }
}

DirectX::XMVECTOR Contact::calculateFrictionlessImpulse(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::CXMMATRIX inverseInertiaTensor2) {
    using namespace DirectX;

    XMVECTOR relativeContactPosition1XM = XMLoadFloat3(&m_relativeContactPosition[0]);
    XMVECTOR relativeContactPosition2XM = XMLoadFloat3(&m_relativeContactPosition[1]);
    XMVECTOR contactNormalXM = XMLoadFloat3(&contactNormal);

    XMVECTOR deltaVelWorld = XMVector3Cross(relativeContactPosition1XM, contactNormalXM);
    deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, inverseInertiaTensor1);
    deltaVelWorld = XMVector3Cross(deltaVelWorld, relativeContactPosition1XM);

    float deltaVelocity = XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormalXM));

    deltaVelocity += body[0]->getInverseMass();

    if (body[1]) {
        XMVECTOR deltaVelWorld = XMVector3Cross(relativeContactPosition2XM, contactNormalXM);
        deltaVelWorld = XMVector3TransformNormal(deltaVelWorld, inverseInertiaTensor2);
        deltaVelWorld = XMVector3Cross(deltaVelWorld, relativeContactPosition2XM);

        deltaVelocity += XMVectorGetX(XMVector3Dot(deltaVelWorld, contactNormalXM));

        deltaVelocity += body[1]->getInverseMass();
    }

    XMVECTOR impulseContact = XMVectorSet(m_desiredDeltaVelocity / deltaVelocity, 0.0f, 0.0f, 0.0f);
    return impulseContact;
}

DirectX::XMVECTOR Contact::calculateFrictionlessImpulse(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2) {
    return calculateFrictionlessImpulse(DirectX::XMLoadFloat3x3(&inverseInertiaTensor1), DirectX::XMLoadFloat3x3(&inverseInertiaTensor2));
}

DirectX::XMFLOAT3 Contact::calculateFrictionlessImpulse3f(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::CXMMATRIX inverseInertiaTensor2) {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, calculateFrictionlessImpulse(inverseInertiaTensor1, inverseInertiaTensor2));
    return res;
}

DirectX::XMFLOAT3 Contact::calculateFrictionlessImpulse3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2) {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, calculateFrictionlessImpulse(inverseInertiaTensor1, inverseInertiaTensor2));
    return res;
}

DirectX::XMVECTOR Contact::calculateFrictionImpulse(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::FXMMATRIX inverseInertiaTensor2) {
    using namespace DirectX;

    float inverseMass = body[0]->getInverseMass();

    XMVECTOR relativeContactPosition1XM = XMLoadFloat3(&m_relativeContactPosition[0]);
    XMVECTOR relativeContactPosition2XM = XMLoadFloat3(&m_relativeContactPosition[0]);

    XMFLOAT3 v = m_relativeContactPosition[0];
    XMMATRIX impulseToTorque = XMMatrixSet(
        0.0f, -v.z, v.y,  0.0f,
        v.z,  0.0f, -v.x, 0.0f,
        -v.y, v.x,  0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    );

    XMMATRIX deltaVelWorld = impulseToTorque;
    deltaVelWorld *= inverseInertiaTensor1;
    deltaVelWorld *= impulseToTorque;
    deltaVelWorld *= -1.0f;

    if (body[1]) {
        v = m_relativeContactPosition[1];
        impulseToTorque = XMMatrixSet(
            0.0f, -v.z, v.y, 0.0f,
            v.z, 0.0f, -v.x, 0.0f,
            -v.y, v.x, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        );

        XMMATRIX deltaVelWorld2 = impulseToTorque;
        deltaVelWorld2 *= inverseInertiaTensor2;
        deltaVelWorld2 *= impulseToTorque;
        deltaVelWorld2 *= -1;

        deltaVelWorld += deltaVelWorld2;

        inverseMass += body[1]->getInverseMass();
    }

    XMMATRIX contactToWorldXM = XMLoadFloat3x3(&m_contactToWorld);
    XMMATRIX deltaVelocity = XMMatrixTranspose(contactToWorldXM);
    deltaVelocity *= deltaVelWorld;
    deltaVelocity *= contactToWorldXM;

    deltaVelocity.r[0] = XMVectorSetX(deltaVelocity.r[0], XMVectorGetX(deltaVelocity.r[0]) + inverseMass);
    deltaVelocity.r[1] = XMVectorSetY(deltaVelocity.r[1], XMVectorGetY(deltaVelocity.r[1]) + inverseMass);
    deltaVelocity.r[2] = XMVectorSetZ(deltaVelocity.r[2], XMVectorGetZ(deltaVelocity.r[2]) + inverseMass);

    XMMATRIX impulseMatrix = XMMatrixInverse(nullptr, deltaVelocity);
    XMVECTOR velKill = XMVectorSet(m_desiredDeltaVelocity, -m_contactVelocity.y, -m_contactVelocity.z, 0.0f);

    XMVECTOR impulseContact = XMVector3TransformNormal(velKill, impulseMatrix);

    float x = XMVectorGetX(impulseContact);
    float y = XMVectorGetY(impulseContact);
    float z = XMVectorGetZ(impulseContact);
    float planarImpulse = std::sqrtf(y * y + z * z);
    if (planarImpulse > x * friction) {
        impulseContact = XMVectorSetY(impulseContact, y / planarImpulse);
        impulseContact = XMVectorSetZ(impulseContact, z / planarImpulse);

        impulseContact = XMVectorSetX(impulseContact, XMVectorGetX(deltaVelocity.r[0]) + XMVectorGetY(deltaVelocity.r[0]) * friction * XMVectorGetY(impulseContact) + XMVectorGetZ(deltaVelocity.r[0]) * friction * XMVectorGetZ(impulseContact));
        impulseContact = XMVectorSetX(impulseContact, m_desiredDeltaVelocity / XMVectorGetX(impulseContact));
        impulseContact = XMVectorSetY(impulseContact, friction * XMVectorGetX(impulseContact));
        impulseContact = XMVectorSetZ(impulseContact, friction * XMVectorGetX(impulseContact));
    }
    return impulseContact;
}

DirectX::XMVECTOR Contact::calculateFrictionImpulse(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2) {
    return calculateFrictionImpulse(DirectX::XMLoadFloat3x3(&inverseInertiaTensor1), DirectX::XMLoadFloat3x3(&inverseInertiaTensor2));
}

DirectX::XMFLOAT3 Contact::calculateFrictionImpulse3f(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::FXMMATRIX inverseInertiaTensor2) {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, calculateFrictionImpulse(inverseInertiaTensor1, inverseInertiaTensor2));
    return res;
}

DirectX::XMFLOAT3 Contact::calculateFrictionImpulse3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2) {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, calculateFrictionImpulse(inverseInertiaTensor1, inverseInertiaTensor2));
    return res;
}

void Contact::setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution) {
    body[0] = one;
    body[1] = two;
    Contact::friction = friction;
    Contact::restitution = restitution;
}

void Contact::calculateInternals(float duration) {
    using namespace DirectX;

    if (!body[0]) {
        swapBodies();
    }
    calculateContactBasis();

    XMStoreFloat3(&m_relativeContactPosition[0], XMLoadFloat3(&contactPoint) - body[0]->getPosition());
    if (body[1]) {
        XMStoreFloat3(&m_relativeContactPosition[1], XMLoadFloat3(&contactPoint) - body[1]->getPosition());
    }

    m_contactVelocity = calculateLocalVelocity3f(0, duration);
    if (body[1]) {
        XMStoreFloat3(&m_contactVelocity, XMLoadFloat3(&m_contactVelocity) - calculateLocalVelocity(1, duration));
    }

    calculateDesiredDeltaVelocity(duration);
}

void Contact::swapBodies() {
    contactNormal.x *= -1.0f;
    contactNormal.y *= -1.0f;
    contactNormal.z *= -1.0f;

    RigidBody* temp = body[0];
    body[0] = body[1];
    body[1] = temp;
}

void Contact::matchAwakeState() {
    if (!body[1]) { return; }

    bool body0awake = body[0]->getAwake();
    bool body1awake = body[1]->getAwake();

    if (body0awake ^ body1awake) {
        if (body0awake) {
            body[1]->setAwake();
        }
        else {
            body[0]->setAwake();
        }
    }
}

void Contact::calculateDesiredDeltaVelocity(float duration) {
    using namespace DirectX;
    const static float velocityLimit = 0.25f;

    float velocityFromAcc = 0;
    DirectX::XMVECTOR contactNormalXM = XMLoadFloat3(&contactNormal);

    if (body[0]->getAwake()) {
        velocityFromAcc += XMVectorGetX(XMVector3Dot(body[0]->getLastFrameAcceleration() * duration, contactNormalXM));
    }

    if (body[1] && body[1]->getAwake()) {
        velocityFromAcc -= XMVectorGetX(XMVector3Dot(body[1]->getLastFrameAcceleration() * duration, contactNormalXM));
    }

    float thisRestitution = restitution;
    if (std::fabsf(m_contactVelocity.x) < velocityLimit) {
        thisRestitution = 0.0f;
    }

    m_desiredDeltaVelocity = -m_contactVelocity.x - thisRestitution * (m_contactVelocity.x - velocityFromAcc);
}

DirectX::XMVECTOR Contact::calculateLocalVelocity(unsigned bodyIndex, float duration) {
    using namespace DirectX;

    RigidBody* thisBody = body[bodyIndex];

    XMVECTOR velocity = XMVector3Cross(thisBody->getRotation(), XMLoadFloat3(&m_relativeContactPosition[bodyIndex]));
    velocity += thisBody->getVelocity();

    XMMATRIX contactToWorldTransposedXM = XMMatrixTranspose(XMLoadFloat3x3(&m_contactToWorld));

    XMVECTOR contactVelocity = XMVector3TransformNormal(velocity, contactToWorldTransposedXM);
    XMVECTOR accVelocity = thisBody->getLastFrameAcceleration() * duration;
    accVelocity = XMVector3TransformNormal(accVelocity, contactToWorldTransposedXM);
    accVelocity = XMVectorSetX(accVelocity, 0.0f);
    contactVelocity += accVelocity;

    return contactVelocity;
}

DirectX::XMFLOAT3 Contact::calculateLocalVelocity3f(unsigned bodyIndex, float duration) {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, calculateLocalVelocity(bodyIndex, duration));
    return res;
}

void Contact::calculateContactBasis() {
    DirectX::XMFLOAT3 contactTangent[2];

    if (std::fabsf(contactNormal.x) > std::fabsf(contactNormal.y)) {
        const float s = 1.0f / std::sqrtf(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

        contactTangent[0].x = contactNormal.z * s;
        contactTangent[0].y = 0;
        contactTangent[0].z = -contactNormal.x * s;

        contactTangent[1].x = contactNormal.y * contactTangent[0].x;
        contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
    }
    else {
        const float s = 1.0f / std::sqrtf(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

        contactTangent[0].x = 0;
        contactTangent[0].y = -contactNormal.z * s;
        contactTangent[0].z = contactNormal.y * s;

        contactTangent[1].x = contactNormal.y * contactTangent[0].z - contactNormal.z * contactTangent[0].y;
        contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = contactNormal.x * contactTangent[0].y;
    }

    m_contactToWorld._11 = contactNormal.x;
    m_contactToWorld._12 = contactNormal.y;
    m_contactToWorld._13 = contactNormal.z;

    m_contactToWorld._21 = contactTangent[0].x;
    m_contactToWorld._22 = contactTangent[0].y;
    m_contactToWorld._23 = contactTangent[0].z;

    m_contactToWorld._31 = contactTangent[1].x;
    m_contactToWorld._32 = contactTangent[1].y;
    m_contactToWorld._33 = contactTangent[1].z;
}
