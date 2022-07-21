#include "spring.h"
#include <cmath>

Spring::Spring(const DirectX::XMFLOAT3& localConnectionPt, RigidBody* other, const DirectX::XMFLOAT3& otherConnectionPt, float springConstant, float restLength) : m_connectionPoint(localConnectionPt), m_otherConnectionPoint(otherConnectionPt), m_other(other), m_springConstant(springConstant), m_restLength(restLength) {}

Spring::Spring(DirectX::FXMVECTOR localConnectionPt, RigidBody* other, DirectX::FXMVECTOR otherConnectionPt, float springConstant, float restLength) : m_other(other), m_springConstant(springConstant), m_restLength(restLength) {
	DirectX::XMStoreFloat3(&m_connectionPoint, localConnectionPt);
	DirectX::XMStoreFloat3(&m_otherConnectionPoint, otherConnectionPt);
}

void Spring::updateForce(RigidBody* body, float duration) {
    using namespace DirectX;
    
    XMVECTOR lws = body->getPointInWorldSpace(m_connectionPoint);
    XMVECTOR ows = m_other->getPointInWorldSpace(m_otherConnectionPoint);

    XMVECTOR force = lws - ows;

    float magnitude = XMVectorGetX(XMVector3Length(force));
    magnitude = std::fabsf(magnitude - m_restLength);
    magnitude *= m_springConstant;

    force = XMVector3Normalize(force);
    force *= -magnitude;
    body->addForceAtPoint(force, lws);
}
