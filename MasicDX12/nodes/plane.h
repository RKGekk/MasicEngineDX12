#pragma once

#include <DirectXMath.h>

class Plane {
private:
	DirectX::XMFLOAT4 m_coefficients;

public:
	void Normalize();

	void Init(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2);
	void Init(DirectX::FXMVECTOR p0, DirectX::FXMVECTOR p1, DirectX::FXMVECTOR p2);
	bool Inside(const DirectX::XMFLOAT3& point, float radius) const;
	bool Inside(const DirectX::XMVECTOR point, float radius) const;
	bool Inside(const DirectX::XMFLOAT3& point) const;
	bool Inside(DirectX::XMVECTOR point) const;
};