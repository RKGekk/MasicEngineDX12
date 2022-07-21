#pragma once

#include <DirectXMath.h>

#include "rigid_body.h"

class ContactResolver;

class Contact {
    friend class ContactResolver;

public:
    RigidBody* body[2];
    float friction;
    float restitution;
    DirectX::XMFLOAT3 contactPoint;
    DirectX::XMFLOAT3 contactNormal;
    float penetration;

    void setBodyData(RigidBody* one, RigidBody* two, float friction, float restitution);

protected:
    DirectX::XMFLOAT3X3 m_contactToWorld;
    DirectX::XMFLOAT3 m_contactVelocity;
    float m_desiredDeltaVelocity;
    DirectX::XMFLOAT3 m_relativeContactPosition[2];

    void calculateInternals(float duration);
    void swapBodies();
    void matchAwakeState();
    void calculateDesiredDeltaVelocity(float duration);
    DirectX::XMVECTOR calculateLocalVelocity(unsigned bodyIndex, float duration);
    DirectX::XMFLOAT3 calculateLocalVelocity3f(unsigned bodyIndex, float duration);
    void calculateContactBasis();
    void applyImpulse(DirectX::FXMVECTOR impulse, RigidBody* body, DirectX::XMFLOAT3* velocityChange, DirectX::XMFLOAT3* rotationChange);
    void applyVelocityChange(DirectX::XMFLOAT3 velocityChange[2], DirectX::XMFLOAT3 rotationChange[2]);
    void applyPositionChange(DirectX::XMFLOAT3 linearChange[2], DirectX::XMFLOAT3 angularChange[2], float penetration);
    DirectX::XMVECTOR calculateFrictionlessImpulse(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::CXMMATRIX inverseInertiaTensor2);
    DirectX::XMVECTOR calculateFrictionlessImpulse(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2);
    DirectX::XMFLOAT3 calculateFrictionlessImpulse3f(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::CXMMATRIX inverseInertiaTensor2);
    DirectX::XMFLOAT3 calculateFrictionlessImpulse3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2);
    DirectX::XMVECTOR calculateFrictionImpulse(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::FXMMATRIX inverseInertiaTensor2);
    DirectX::XMVECTOR calculateFrictionImpulse(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2);
    DirectX::XMFLOAT3 calculateFrictionImpulse3f(DirectX::FXMMATRIX inverseInertiaTensor1, DirectX::FXMMATRIX inverseInertiaTensor2);
    DirectX::XMFLOAT3 calculateFrictionImpulse3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor1, const DirectX::XMFLOAT3X3& inverseInertiaTensor2);
};