#include "aero.h"

Aero::Aero(const DirectX::XMFLOAT3X3& tensor, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed) {
    m_tensor = tensor;
    m_position = position;
    m_windspeed = windspeed;
}

Aero::Aero(DirectX::FXMMATRIX tensor, const DirectX::CXMVECTOR position, const DirectX::XMFLOAT3* windspeed) {
    DirectX::XMStoreFloat3x3(&m_tensor, tensor);
    DirectX::XMStoreFloat3(&m_position, position);
    m_windspeed = windspeed;
}

void Aero::updateForce(RigidBody* body, float duration) {
    updateForceFromTensor(body, duration, m_tensor);
}

void Aero::updateForceFromTensor(RigidBody* body, float duration, const DirectX::XMFLOAT3X3& tensor) {
    updateForceFromTensor(body, duration, DirectX::XMLoadFloat3x3(&tensor));
}

void Aero::updateForceFromTensor(RigidBody* body, float duration, DirectX::FXMMATRIX tensor) {
    using namespace DirectX;

    XMVECTOR velocity = body->getVelocity();
    velocity += XMLoadFloat3(&*m_windspeed);

    XMMATRIX bodyTransformXM = body->getTransform();
    XMVECTOR bodyVel = XMVector3TransformNormal(velocity, XMMatrixTranspose(bodyTransformXM));

    XMVECTOR bodyForce = XMVector3TransformNormal(bodyVel, tensor);
    XMVECTOR force = XMVector3TransformNormal(bodyForce, bodyTransformXM);

    body->addForceAtBodyPoint(force, m_position);
}
