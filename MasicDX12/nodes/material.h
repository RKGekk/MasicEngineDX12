#pragma once

#include <DirectXMath.h>
#include <Windows.h>

#include "material_dx.h"

class Material {
private:
	MaterialDX m_D3DMaterial;

public:
	Material();
	void SetAmbient(DirectX::FXMVECTOR color);
	void SetAmbient(DirectX::XMFLOAT3 color);
	DirectX::XMVECTOR GetAmbient() const;
	const DirectX::XMFLOAT3& GetAmbient3() const;

	void SetDiffuse(DirectX::FXMVECTOR color);
	void SetDiffuse(DirectX::XMFLOAT4 color);
	DirectX::XMVECTOR GetDiffuse() const;
	const DirectX::XMFLOAT4& GetDiffuse4() const;

	void SetSpecular(DirectX::FXMVECTOR color, const float power);
	void SetSpecular(DirectX::XMFLOAT3 color, const float power);
	DirectX::XMVECTOR GetSpecular() const;
	const DirectX::XMFLOAT3& GetSpecular3() const;

	void SetEmissive(DirectX::FXMVECTOR color);
	void SetEmissive(DirectX::XMFLOAT3 color);
	DirectX::XMVECTOR GetEmissive() const;
	const DirectX::XMFLOAT3& GetEmissive3() const;

	void SetAlpha(const float alpha);
	bool HasAlpha() const;
	float GetAlpha() const;
};