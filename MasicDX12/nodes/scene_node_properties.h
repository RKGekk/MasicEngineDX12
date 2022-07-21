#pragma once

#include <string>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "render_pass.h"
#include "material.h"
#include "alpha_type.h"

class SceneNodeProperties {
	friend class SceneNode;

protected:
	ActorId m_ActorId;
	ComponentId m_ComponentId;
	std::string m_Name;
	DirectX::XMFLOAT4X4 m_ToWorld;
	DirectX::XMFLOAT4X4 m_FromWorld;
	DirectX::XMFLOAT3 m_scale;
	float m_Radius;
	bool m_active;

	RenderPass m_RenderPass;
	Material m_Material;
	AlphaType m_AlphaType;

	void SetAlpha(const float alpha);

public:
	SceneNodeProperties();

	const ActorId ActorId() const;
	const ComponentId ComponentId() const;

	const DirectX::XMFLOAT4X4& ToWorld4x4() const;
	DirectX::XMMATRIX ToWorld() const;

	DirectX::XMFLOAT4 Position4() const;
	DirectX::XMFLOAT3 Position3() const;
	DirectX::XMVECTOR Position() const;

	const DirectX::XMFLOAT3& Scale3() const;
	DirectX::XMVECTOR Scale() const;
	float MaxScale() const;

	const DirectX::XMFLOAT4X4& FromWorld4x4() const;
	DirectX::XMMATRIX FromWorld() const;

	const char* NameCstr() const;
	const std::string& Name() const;

	bool HasAlpha() const;
	float Alpha() const;
	AlphaType AlphaType() const;

	RenderPass RenderPass() const;
	float Radius() const;

	const Material& GetMaterial() const;
};