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
	DirectX::XMFLOAT4X4 m_to_world_cumulative;
	DirectX::XMFLOAT4X4 m_from_world_cumulative;

	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT3 m_scale_cumulative;
	
	bool m_active;

	Material m_material;
	AlphaType m_alpha_type;

	void SetAlpha(const float alpha);

public:
	SceneNodeProperties();

	DirectX::XMMATRIX ToWorld() const;
	const DirectX::XMFLOAT4X4& ToWorld4x4() const;
	const DirectX::XMFLOAT4X4& ToWorld4x4T() const;

	DirectX::XMMATRIX CumulativeToWorld() const;
	const DirectX::XMFLOAT4X4& CumulativeToWorld4x4() const;
	const DirectX::XMFLOAT4X4& CumulativeToWorld4x4T() const;

	DirectX::XMVECTOR Position() const;
	DirectX::XMFLOAT4 Position4() const;
	DirectX::XMFLOAT3 Position3() const;

	DirectX::XMVECTOR CumulativePosition() const;
	DirectX::XMFLOAT4 CumulativePosition4() const;
	DirectX::XMFLOAT3 CumulativePosition3() const;

	DirectX::XMVECTOR Direction() const;
	DirectX::XMFLOAT3 Direction3f() const;
	DirectX::XMVECTOR Up() const;
	DirectX::XMFLOAT3 Up3f() const;

	DirectX::XMVECTOR CumulativeDirection() const;
	DirectX::XMFLOAT3 CumulativeDirection3f() const;
	DirectX::XMVECTOR CumulativeUp() const;
	DirectX::XMFLOAT3 CumulativeUp3f() const;

	const DirectX::XMFLOAT3& Scale3() const;
	DirectX::XMVECTOR Scale() const;
	float MaxScale() const;

	const DirectX::XMFLOAT3& CumulativeScale3() const;
	DirectX::XMVECTOR CumulativeScale() const;
	float CumulativeMaxScale() const;

	DirectX::XMMATRIX FromWorld() const;
	const DirectX::XMFLOAT4X4& FromWorld4x4() const;
	const DirectX::XMFLOAT4X4& FromWorld4x4T() const;

	DirectX::XMMATRIX CumulativeFromWorld() const;
	const DirectX::XMFLOAT4X4& CumulativeFromWorld4x4() const;
	const DirectX::XMFLOAT4X4& CumulativeFromWorld4x4T() const;

	const char* NameCstr() const;
	const std::string& Name() const;

	bool HasAlpha() const;
	float Alpha() const;
	AlphaType AlphaType() const;

	const Material& GetMaterial() const;
};