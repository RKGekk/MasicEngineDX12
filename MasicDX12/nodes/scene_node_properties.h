#pragma once

#include <string>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "../graphics/material.h"
#include "alpha_type.h"

class SceneNodeProperties {
	friend class SceneNode;

protected:
	std::string m_name;
	DirectX::XMFLOAT4X4 m_to_world;
	DirectX::XMFLOAT4X4 m_from_world;
	DirectX::XMFLOAT3 m_scale;
	
	bool m_active;

	Material m_material;
	AlphaType m_alpha_type;

	void SetAlpha(const float alpha);

public:
	SceneNodeProperties();

	DirectX::XMMATRIX ToWorld() const;
	const DirectX::XMFLOAT4X4& ToWorld4x4() const;
	const DirectX::XMFLOAT4X4& ToWorld4x4T() const;

	DirectX::XMFLOAT4 Position4() const;
	DirectX::XMFLOAT3 Position3() const;
	DirectX::XMVECTOR Position() const;

	const DirectX::XMFLOAT3& Scale3() const;
	DirectX::XMVECTOR Scale() const;
	float MaxScale() const;

	DirectX::XMMATRIX FromWorld() const;
	const DirectX::XMFLOAT4X4& FromWorld4x4() const;
	const DirectX::XMFLOAT4X4& FromWorld4x4T() const;

	const char* NameCstr() const;
	const std::string& Name() const;

	bool HasAlpha() const;
	float Alpha() const;
	AlphaType AlphaType() const;

	const Material& GetMaterial() const;
};