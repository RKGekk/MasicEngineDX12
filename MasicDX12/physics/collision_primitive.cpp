#include "collision_primitive.h"

void CollisionPrimitive::calculateInternals() {
	using namespace DirectX;
	XMStoreFloat4x4(&m_transform, XMMatrixMultiply(body->getTransform(), XMLoadFloat4x4(&offset)));
}

DirectX::XMVECTOR CollisionPrimitive::getAxis(unsigned index) const {
	return DirectX::XMVectorSet(m_transform.m[index][0], m_transform.m[index][1], m_transform.m[index][2], 0.0f);
}

DirectX::XMFLOAT3 CollisionPrimitive::getAxis3f(unsigned index) const {
	return { m_transform.m[index][0], m_transform.m[index][1], m_transform.m[index][2] };
}

const DirectX::XMFLOAT4X4& CollisionPrimitive::getTransform4x4f() const {
	return m_transform;
}

DirectX::XMMATRIX CollisionPrimitive::getTransform() const {
	return DirectX::XMLoadFloat4x4(&m_transform);
}
