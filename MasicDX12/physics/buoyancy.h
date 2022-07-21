#pragma once

#include <DirectXMath.h>

#include "force_generator.h"

class Buoyancy : public ForceGenerator {
    float m_maxDepth;
    float m_volume;
    float m_waterHeight;
    float m_liquidDensity;
    DirectX::XMFLOAT3 m_centreOfBuoyancy;

public:
    Buoyancy(const DirectX::XMFLOAT3& cOfB, float maxDepth, float volume, float waterHeight, float liquidDensity = 1000.0f);
    Buoyancy(DirectX::FXMVECTOR cOfB, float maxDepth, float volume, float waterHeight, float liquidDensity = 1000.0f);

    virtual void updateForce(RigidBody* body, float duration) override;
};