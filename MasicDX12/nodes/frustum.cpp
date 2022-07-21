#include "frustum.h"

Frustum::Frustum() {
	m_Fov = DirectX::XM_PI / 4.0f;
	m_Aspect = 1.0f;
	m_Near = 1.0f;
	m_Far = 1000.0f;
}

bool Frustum::Inside(const DirectX::XMFLOAT3& point) const {
	for (int i = 0; i < 6; ++i) {
		if (!m_Planes[i].Inside(point)) {
			return false;
		}
	}

	return true;
}

bool Frustum::Inside(DirectX::XMVECTOR point) const {
	for (int i = 0; i < 6; ++i) {
		if (!m_Planes[i].Inside(point)) {
			return false;
		}
	}

	return true;
}

bool Frustum::Inside(const DirectX::XMFLOAT3& point, const float radius) const {
	for (int i = 0; i < 6; ++i) {
		if (!m_Planes[i].Inside(point, radius)) {
			return false;
		}
	}

	return true;
}

bool Frustum::Inside(DirectX::XMVECTOR point, const float radius) const {
	for (int i = 0; i < 6; ++i) {
		if (!m_Planes[i].Inside(point, radius)) {
			return false;
		}
	}

	return true;
}

const Plane& Frustum::Get(Side side) {
	switch (side) {
		case Frustum::Side::Near: return m_Planes[0];
		case Frustum::Side::Far: return m_Planes[1];
		case Frustum::Side::Top: return m_Planes[2];
		case Frustum::Side::Right: return m_Planes[3];
		case Frustum::Side::Bottom: return m_Planes[4];
		case Frustum::Side::Left: return m_Planes[5];
		default: return m_Planes[0];
	}
}

void Frustum::SetFOV(float fov) {
	m_Fov = fov;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustum::SetAspect(float aspect) {
	m_Aspect = aspect;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustum::SetNear(float nearClip) {
	m_Near = nearClip;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustum::SetFar(float farClip) {
	m_Far = farClip;
	Init(m_Fov, m_Aspect, m_Near, m_Far);
}

void Frustum::Init(float fov, float aspect, float nearClip, float farClip) {
	using namespace DirectX;

	m_Fov = fov;
	m_Aspect = aspect;
	m_Near = nearClip;
	m_Far = farClip;

	DirectX::XMVECTOR right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	float tanFovOver2 = DirectX::XMVectorGetX(DirectX::XMVectorTan(DirectX::XMVectorReplicate(m_Fov / 2.0f)));
	DirectX::XMVECTOR nearRight = DirectX::XMVectorScale(right, (m_Near * tanFovOver2) * m_Aspect);
	DirectX::XMVECTOR farRight = DirectX::XMVectorScale(right, (m_Far * tanFovOver2) * m_Aspect);
	DirectX::XMVECTOR nearUp = DirectX::XMVectorScale(up, m_Near * tanFovOver2);
	DirectX::XMVECTOR farUp = DirectX::XMVectorScale(up, m_Far * tanFovOver2);

	DirectX::XMStoreFloat3(&m_NearClip[0], (m_Near * forward) - nearRight + nearUp);
	DirectX::XMStoreFloat3(&m_NearClip[1], (m_Near * forward) + nearRight + nearUp);
	DirectX::XMStoreFloat3(&m_NearClip[2], (m_Near * forward) + nearRight - nearUp);
	DirectX::XMStoreFloat3(&m_NearClip[3], (m_Near * forward) - nearRight - nearUp);

	DirectX::XMStoreFloat3(&m_FarClip[0], (m_Far * forward) - farRight + farUp);
	DirectX::XMStoreFloat3(&m_FarClip[1], (m_Far * forward) + farRight + farUp);
	DirectX::XMStoreFloat3(&m_FarClip[2], (m_Far * forward) + farRight - farUp);
	DirectX::XMStoreFloat3(&m_FarClip[3], (m_Far * forward) - farRight - farUp);

	DirectX::XMFLOAT3 origin(0.0f, 0.0f, 0.0f);
	m_Planes[0].Init(m_NearClip[2], m_NearClip[1], m_NearClip[0]);
	m_Planes[1].Init(m_FarClip[0], m_FarClip[1], m_FarClip[2]);
	m_Planes[3].Init(m_FarClip[2], m_FarClip[1], origin);
	m_Planes[2].Init(m_FarClip[1], m_FarClip[0], origin);
	m_Planes[5].Init(m_FarClip[0], m_FarClip[3], origin);
	m_Planes[4].Init(m_FarClip[3], m_FarClip[2], origin);
}

void Frustum::Render() {}