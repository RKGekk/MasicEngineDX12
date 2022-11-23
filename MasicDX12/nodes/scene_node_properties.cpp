#include "scene_node_properties.h"

SceneNodeProperties::SceneNodeProperties() {
	DirectX::XMStoreFloat4x4(&m_to_parent, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_to_root, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_from_parent, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_from_root, DirectX::XMMatrixIdentity());
	m_active = true;
	m_dirty_flags = to_underlying(SceneNodeProperties::DirtyFlags::DF_All);
	m_generation = 0u;
	m_group_id = 0u;
	m_AABB = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere, m_AABB);
	m_AABB_cumulative = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere_cumulative, m_AABB_cumulative);
}

DirectX::XMMATRIX SceneNodeProperties::ToParent() const {
	return DirectX::XMLoadFloat4x4(&m_to_parent);
}

DirectX::XMMATRIX SceneNodeProperties::ToParentT() const {
	return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_to_parent));
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::ToParent4x4() const {
	return m_to_parent;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::ToParent4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(ToParent()));
	return res;
}

DirectX::XMMATRIX SceneNodeProperties::ToRoot() const {
	return DirectX::XMLoadFloat4x4(&m_to_root);
}

DirectX::XMMATRIX SceneNodeProperties::ToRootT() const {
	return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_to_root));
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::ToRoot4x4() const {
	return m_to_root;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::ToRoot4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(ToRoot()));
	return res;
}

DirectX::XMVECTOR SceneNodeProperties::ToParentTranslation() const {
	return DirectX::XMVectorSet(m_to_parent.m[3][0], m_to_parent.m[3][1], m_to_parent.m[3][2], 1.0f);
}

DirectX::XMFLOAT4 SceneNodeProperties::ToParentTranslation4() const {
	return DirectX::XMFLOAT4(m_to_parent.m[3][0], m_to_parent.m[3][1], m_to_parent.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToParentTranslation3() const {
	return DirectX::XMFLOAT3(m_to_parent.m[3][0], m_to_parent.m[3][1], m_to_parent.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::ToRootTranslation() const {
	return DirectX::XMVectorSet(m_to_root.m[3][0], m_to_root.m[3][1], m_to_root.m[3][2], 1.0f);
}

DirectX::XMFLOAT4 SceneNodeProperties::ToRootTranslation4() const {
	return DirectX::XMFLOAT4(m_to_root.m[3][0], m_to_root.m[3][1], m_to_root.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToRootTranslation3() const {
	return DirectX::XMFLOAT3(m_to_root.m[3][0], m_to_root.m[3][1], m_to_root.m[3][2]);
}

DirectX::XMVECTOR SceneNodeProperties::ToParentDirection() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_parent;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return DirectX::XMVector4Transform(forward, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToParentDirection3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, ToParentDirection());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::ToParentUp() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_parent;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMVector4Transform(up, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToParentUp3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, ToParentUp());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::ToRootDirection() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_root;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return DirectX::XMVector4Transform(forward, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToRootDirection3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, ToRootDirection());
	return result;
}

DirectX::XMVECTOR SceneNodeProperties::ToRootUp() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_to_root;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMVector4Transform(up, just_rot);
}

DirectX::XMFLOAT3 SceneNodeProperties::ToRootUp3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, ToRootUp());
	return result;
}

DirectX::XMMATRIX SceneNodeProperties::FromParent() const {
	return DirectX::XMLoadFloat4x4(&m_from_parent);
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::FromParent4x4() const {
	return m_from_parent;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::FromParent4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(FromParent()));
	return res;
}

DirectX::XMMATRIX SceneNodeProperties::FromRoot() const {
	return DirectX::XMLoadFloat4x4(&m_from_root);
}

const DirectX::XMFLOAT4X4& SceneNodeProperties::FromRoot4x4() const {
	return m_from_root;
}

DirectX::XMFLOAT4X4 SceneNodeProperties::FromRoot4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(FromRoot()));
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

uint32_t SceneNodeProperties::GetGeneration() const {
	return m_generation;
}

const DirectX::BoundingBox& SceneNodeProperties::AABB() const {
	return m_AABB;
}

const DirectX::BoundingSphere& SceneNodeProperties::Sphere() const {
	return m_sphere;
}

const DirectX::BoundingBox& SceneNodeProperties::CumulativeAABB() const {
	return m_AABB_cumulative;
}

const DirectX::BoundingSphere& SceneNodeProperties::CumulativeSphere() const {
	return m_sphere_cumulative;
}

const DirectX::BoundingBox& SceneNodeProperties::MergedAABB() const {
	return m_AABB_merged;
}

const DirectX::BoundingSphere& SceneNodeProperties::MergedSphere() const {
	return m_sphere_merged;
}
