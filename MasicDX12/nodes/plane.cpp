#include "plane.h"

void Plane::Normalize() {
	DirectX::XMStoreFloat4(&m_coefficients, DirectX::XMPlaneNormalize(DirectX::XMLoadFloat4(&m_coefficients)));
}

void Plane::Init(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2) {
	DirectX::XMStoreFloat4(&m_coefficients, DirectX::XMPlaneFromPoints(DirectX::XMLoadFloat3(&p0), DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p2)));
}

void Plane::Init(DirectX::FXMVECTOR p0, DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2) {
	DirectX::XMStoreFloat4(&m_coefficients, DirectX::XMPlaneFromPoints(p0, p1, p2));
}

bool Plane::Inside(const DirectX::XMFLOAT3& point, float radius) const {
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&m_coefficients), DirectX::XMLoadFloat3(&point))) >= -radius;
}

bool Plane::Inside(const DirectX::XMVECTOR point, float radius) const {
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&m_coefficients), point)) >= -radius;
}

bool Plane::Inside(const DirectX::XMFLOAT3& point) const {
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&m_coefficients), DirectX::XMLoadFloat3(&point))) >= 0.0f;
}

bool Plane::Inside(DirectX::XMVECTOR point) const {
	return DirectX::XMVectorGetX(DirectX::XMPlaneDotCoord(DirectX::XMLoadFloat4(&m_coefficients), point)) >= 0.0f;
}