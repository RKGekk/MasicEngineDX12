#include "scene_node_properties.h"

void SceneNodeProperties::SetAlpha(const float alpha) {
	if (alpha != 1.0f) {
		m_alpha_type = AlphaType::AlphaMaterial;
		m_material.SetOpacity(alpha);
	}
	else {
		m_alpha_type = AlphaType::AlphaOpaque;
		m_material.SetOpacity(alpha);
	}
}

SceneNodeProperties::SceneNodeProperties() {
	m_alpha_type = AlphaType::AlphaOpaque;
	DirectX::XMStoreFloat4x4(&m_to_world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_from_world, DirectX::XMMatrixIdentity());
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_active = true;
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::ToWorld4x4() const {
	return m_to_world;
}

DirectX::XMMATRIX SceneNodeProperties::ToWorld() const {
	return DirectX::XMLoadFloat4x4(&m_to_world);
}

DirectX::XMFLOAT4 SceneNodeProperties::Position4() const {
	return DirectX::XMFLOAT4(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::Position3() const {
	return DirectX::XMFLOAT3(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::Position() const {
	return DirectX::XMVectorSet(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2], 1.0f);
}

const DirectX::XMFLOAT3& SceneNodeProperties::Scale3() const {
	return m_scale;
}

DirectX::XMVECTOR SceneNodeProperties::Scale() const {
	return DirectX::XMLoadFloat3(&m_scale);
}

float SceneNodeProperties::MaxScale() const {
	float scale = m_scale.x;
	scale = scale > m_scale.y ? scale : m_scale.y;
	scale = scale > m_scale.z ? scale : m_scale.z;
	return scale;
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::FromWorld4x4() const {
	return m_from_world;
}

DirectX::XMMATRIX SceneNodeProperties::FromWorld() const {
	return DirectX::XMLoadFloat4x4(&m_from_world);
}

const char* SceneNodeProperties::NameCstr() const {
	return m_name.c_str();
}

const std::string& SceneNodeProperties::Name() const {
	return m_name;
}

bool SceneNodeProperties::HasAlpha() const {
	return m_material.IsTransparent();
}

float SceneNodeProperties::Alpha() const {
	return m_material.GetOpacity();
}

AlphaType SceneNodeProperties::AlphaType() const {
	return m_alpha_type;
}

const Material& SceneNodeProperties::GetMaterial() const {
	return m_material;
}