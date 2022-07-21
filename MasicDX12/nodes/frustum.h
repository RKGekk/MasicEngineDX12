#pragma once

#include <DirectXMath.h>
//DirectX::BoundingFrustum #include <DirectXCollision.h>

#include "plane.h"

class Frustum {
public:
	enum class Side {
		Near = 0,
		Far = 1,
		Top = 2,
		Right = 3,
		Bottom = 4,
		Left = 5,
		NumPlanes = 6
	};

	Plane m_Planes[6];
	DirectX::XMFLOAT3 m_NearClip[4];
	DirectX::XMFLOAT3 m_FarClip[4];

	float m_Fov;
	float m_Aspect;
	float m_Near;
	float m_Far;

public:
	Frustum();

	bool Inside(const DirectX::XMFLOAT3& point) const;
	bool Inside(DirectX::XMVECTOR point) const;
	bool Inside(const DirectX::XMFLOAT3& point, const float radius) const;
	bool Inside(DirectX::XMVECTOR point, const float radius) const;

	const Plane& Get(Side side);
	void SetFOV(float fov);
	void SetAspect(float aspect);
	void SetNear(float nearClip);
	void SetFar(float farClip);

	void Init(float fov, float aspect, float nearClip, float farClip);
	void Render();
};