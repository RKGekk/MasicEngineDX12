#pragma once

#include <DirectXMath.h>

#include "rigid_body.h"

class CollisionPrimitive {
public:
    friend class IntersectionTests;
    friend class CollisionDetector;

    RigidBody* body;
    DirectX::XMFLOAT4X4 offset;

    void calculateInternals();

    DirectX::XMVECTOR getAxis(unsigned index) const;
    DirectX::XMFLOAT3 getAxis3f(unsigned index) const;

    const DirectX::XMFLOAT4X4& getTransform4x4f() const;
    DirectX::XMMATRIX getTransform() const;

protected:
    DirectX::XMFLOAT4X4 m_transform;
};