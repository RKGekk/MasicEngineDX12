#include "scene_node_properties.h"
//#include "../actors/mesh_render_component.h"

void SceneNodeProperties::SetAlpha(const float alpha) {
	if (alpha != 1.0f) {
		m_AlphaType = AlphaType::AlphaMaterial;
		m_Material.SetAlpha(alpha);
	}
	else {
		m_AlphaType = AlphaType::AlphaOpaque;
		m_Material.SetAlpha(alpha);
	}
}

SceneNodeProperties::SceneNodeProperties() {
	m_ActorId = INVALID_ACTOR_ID;
	//m_ComponentId = MeshRenderComponent::GetIdFromName(MeshRenderComponent::g_Name);
	m_ComponentId = INVALID_COMPONENT_ID;
	m_Radius = 0;
	m_RenderPass = RenderPass::RenderPass_0;
	m_AlphaType = AlphaType::AlphaOpaque;
	DirectX::XMStoreFloat4x4(&m_ToWorld, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_FromWorld, DirectX::XMMatrixIdentity());
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_active = true;
}

const ActorId SceneNodeProperties::ActorId() const {
	return m_ActorId;
}

const ComponentId SceneNodeProperties::ComponentId() const {
	return m_ComponentId;
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::ToWorld4x4() const {
	return m_ToWorld;
}

DirectX::XMMATRIX SceneNodeProperties::ToWorld() const {
	return DirectX::XMLoadFloat4x4(&m_ToWorld);
}

DirectX::XMFLOAT4 SceneNodeProperties::Position4() const {
	return DirectX::XMFLOAT4(m_ToWorld.m[3][0], m_ToWorld.m[3][1], m_ToWorld.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::Position3() const {
	return DirectX::XMFLOAT3(m_ToWorld.m[3][0], m_ToWorld.m[3][1], m_ToWorld.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::Position() const {
	return DirectX::XMVectorSet(m_ToWorld.m[3][0], m_ToWorld.m[3][1], m_ToWorld.m[3][2], 1.0f);
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
	return m_FromWorld;
}

DirectX::XMMATRIX SceneNodeProperties::FromWorld() const {
	return DirectX::XMLoadFloat4x4(&m_FromWorld);
}

const char* SceneNodeProperties::NameCstr() const {
	return m_Name.c_str();
}

const std::string& SceneNodeProperties::Name() const {
	return m_Name;
}

bool SceneNodeProperties::HasAlpha() const {
	return m_Material.HasAlpha();
}

float SceneNodeProperties::Alpha() const {
	return m_Material.GetAlpha();
}

AlphaType SceneNodeProperties::AlphaType() const {
	return m_AlphaType;
}

RenderPass SceneNodeProperties::RenderPass() const {
	return m_RenderPass;
}

float SceneNodeProperties::Radius() const {
	return m_Radius;
}

const Material& SceneNodeProperties::GetMaterial() const {
	return m_Material;
}