#include "buoyancy.h"

Buoyancy::Buoyancy(const DirectX::XMFLOAT3& cOfB, float maxDepth, float volume, float waterHeight, float liquidDensity) {
    m_centreOfBuoyancy = cOfB;
    m_liquidDensity = liquidDensity;
    m_maxDepth = maxDepth;
    m_volume = volume;
    m_waterHeight = waterHeight;
}

Buoyancy::Buoyancy(DirectX::FXMVECTOR cOfB, float maxDepth, float volume, float waterHeight, float liquidDensity) {
    DirectX::XMStoreFloat3(&m_centreOfBuoyancy, cOfB);
    m_liquidDensity = liquidDensity;
    m_maxDepth = maxDepth;
    m_volume = volume;
    m_waterHeight = waterHeight;
}

void Buoyancy::updateForce(RigidBody* body, float duration) {
    using namespace DirectX;
    float depth = body->getPointInWorldSpace3f(m_centreOfBuoyancy).y;

    if (depth >= m_waterHeight + m_maxDepth) { return; }
    XMVECTOR force = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    if (depth <= m_waterHeight - m_maxDepth) {
        force = XMVectorSetY(force, m_liquidDensity * m_volume);
        body->addForceAtBodyPoint(force, m_centreOfBuoyancy);
        return;
    }
    else {
        force = XMVectorSetY(force, m_liquidDensity * m_volume * (depth - m_maxDepth - m_waterHeight) / 2.0f * m_maxDepth);
        body->addForceAtBodyPoint(force, m_centreOfBuoyancy);
    }
}
