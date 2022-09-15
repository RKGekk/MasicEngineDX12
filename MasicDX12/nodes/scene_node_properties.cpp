#include "scene_node_properties.h"

SceneNodeProperties::SceneNodeProperties() {
	DirectX::XMStoreFloat4x4(&m_to_world, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_from_world, DirectX::XMMatrixIdentity());
	m_scale = { 1.0f, 1.0f, 1.0f };
	m_active = true;
	m_dirty_flags = to_underlying(SceneNodeProperties::DirtyFlags::DF_All);
	m_group_id = 0u;
}

DirectX::XMMATRIX SceneNodeProperties::ToWorld() const {
	return DirectX::XMLoadFloat4x4(&m_to_world);
}


const DirectX::XMFLOAT4X4& SceneNodeProperties::ToWorld4x4() const {
	return m_to_world;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::ToWorld4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(ToWorld()));
	return res;
}

DirectX::XMMATRIX SceneNodeProperties::CumulativeToWorld() const {
	return DirectX::XMLoadFloat4x4(&m_to_world_cumulative);
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::CumulativeToWorld4x4() const {
	return m_to_world_cumulative;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::CumulativeToWorld4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(CumulativeToWorld()));
	return res;
}

DirectX::XMVECTOR SceneNodeProperties::Position() const {
	return DirectX::XMVectorSet(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2], 1.0f);
}

DirectX::XMFLOAT4 SceneNodeProperties::Position4() const {
	return DirectX::XMFLOAT4(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::Position3() const {
	return DirectX::XMFLOAT3(m_to_world.m[3][0], m_to_world.m[3][1], m_to_world.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::CumulativePosition() const {
	return DirectX::XMVectorSet(m_to_world_cumulative.m[3][0], m_to_world_cumulative.m[3][1], m_to_world_cumulative.m[3][2], 1.0f);
}

DirectX::XMFLOAT4 SceneNodeProperties::CumulativePosition4() const {
	return DirectX::XMFLOAT4(m_to_world_cumulative.m[3][0], m_to_world_cumulative.m[3][1], m_to_world_cumulative.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::CumulativePosition3() const {
	return DirectX::XMFLOAT3(m_to_world_cumulative.m[3][0], m_to_world_cumulative.m[3][1], m_to_world_cumulative.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::Direction() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_world;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return DirectX::XMVector4Transform(forward, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::Direction3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, Direction());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::Up() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_world;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMVector4Transform(up, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::Up3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, Up());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::CumulativeDirection() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_world_cumulative;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return DirectX::XMVector4Transform(forward, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::CumulativeDirection3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, CumulativeDirection());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::CumulativeUp() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_world_cumulative;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMVector4Transform(up, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::CumulativeUp3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, CumulativeUp());
	return result;
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

const DirectX::XMFLOAT3& SceneNodeProperties::CumulativeScale3() const {
	return m_scale_cumulative;
}

DirectX::XMVECTOR SceneNodeProperties::CumulativeScale() const {
	return DirectX::XMLoadFloat3(&m_scale_cumulative);
}

float SceneNodeProperties::CumulativeMaxScale() const {
	float scale = m_scale_cumulative.x;
	scale = scale > m_scale_cumulative.y ? scale : m_scale_cumulative.y;
	scale = scale > m_scale_cumulative.z ? scale : m_scale_cumulative.z;
	return scale;
}

DirectX::XMMATRIX SceneNodeProperties::FromWorld() const {
	return DirectX::XMLoadFloat4x4(&m_from_world);
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::FromWorld4x4() const {
	return m_from_world;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::FromWorld4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(FromWorld()));
	return res;
}

DirectX::XMMATRIX SceneNodeProperties::CumulativeFromWorld() const {
	return DirectX::XMLoadFloat4x4(&m_from_world_cumulative);
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::CumulativeFromWorld4x4() const {
	return m_from_world_cumulative;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::CumulativeFromWorld4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(CumulativeFromWorld()));
	return res;
}

const char* SceneNodeProperties::NameCstr() const {
	return m_name.c_str();
}

const std::string& SceneNodeProperties::Name() const {
	return m_name;
}

uint32_t SceneNodeProperties::GetDirtyFlags() const {
	return m_dirty_flags;
}

uint32_t SceneNodeProperties::GetGroupID() const {
	return m_group_id;
}
