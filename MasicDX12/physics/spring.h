#pragma once

#include <DirectXMath.h>

#include "force_generator.h"

class Spring : public ForceGenerator {
    DirectX::XMFLOAT3 m_connectionPoint;
    DirectX::XMFLOAT3 m_otherConnectionPoint;
    RigidBody* m_other;
    float m_springConstant;
    float m_restLength;

public:
    Spring(const DirectX::XMFLOAT3& localConnectionPt, RigidBody* other, const DirectX::XMFLOAT3& otherConnectionPt, float springConstant, float restLength);
    Spring(DirectX::FXMVECTOR localConnectionPt, RigidBody* other, DirectX::FXMVECTOR otherConnectionPt, float springConstant, float restLength);

    virtual void updateForce(RigidBody* body, float duration) override;
};