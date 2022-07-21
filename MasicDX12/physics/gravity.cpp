#include "gravity.h"

Gravity::Gravity(const DirectX::XMFLOAT3& gravity) : m_gravity(gravity){}

Gravity::Gravity(DirectX::FXMVECTOR gravity) {
	DirectX::XMStoreFloat3(&m_gravity, gravity);
}

void Gravity::updateForce(RigidBody* body, float duration) {
    using namespace DirectX;
    if (!body->hasFiniteMass()) { return; }

    body->addForce(DirectX::XMLoadFloat3(&m_gravity) * body->getMass());
}
