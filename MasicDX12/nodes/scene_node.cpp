#include "scene_node.h"
#include "scene.h"
#include "../engine/engine.h"
#include "../actors/transform_component.h"
#include "camera_node.h"

#include <algorithm>

SceneNode::SceneNode(const std::string& name) {
	SetTransform(DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), false);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
}

SceneNode::SceneNode(const std::string& name, uint32_t group_id) {
	SetTransform(DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), false);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
	m_props.m_group_id = group_id;
}

SceneNode::SceneNode(const std::string& name, uint32_t group_id, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from) {
	SetTransform4x4(to, from);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
	m_props.m_group_id = group_id;
}

SceneNode::SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from) {
	SetTransform4x4(to, from);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
}

SceneNode::SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from) {
	SetTransform(to, from, calulate_from);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
}

SceneNode::SceneNode(const std::string& name, uint32_t group_id, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from) {
	SetTransform(to, from, calulate_from);
	m_props.m_name = name;
	m_props.m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_props.m_active = true;
	m_props.m_group_id = group_id;
}

SceneNode::~SceneNode() {}

void SceneNode::Accept(IVisitor& visitor) {
	visitor.Visit(shared_from_this());
	m_props.m_dirty_flags = to_underlying(SceneNodeProperties::DirtyFlags::DF_None);
	for (auto& child : m_children) {
		child->Accept(visitor);
	}
}

HRESULT SceneNode::VOnRestore() {
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->VOnRestore();
		++i;
	}
	return S_OK;
}

HRESULT SceneNode::VOnUpdate() {
	uint32_t dirty_flags = Get().GetDirtyFlags();
	constexpr uint32_t transform_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Transform);
	if (dirty_flags & transform_flag) {
		UpdateCumulativeTransform();
		UpdateMergedAABB();
	}

	auto i = m_children.begin();
	auto end = m_children.end();
	while (i != end) {
		(*i)->VOnUpdate();
		++i;
	}
	m_props.m_dirty_flags = 0u;
	return S_OK;
}

HRESULT SceneNode::VOnLostDevice() {
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->VOnLostDevice();
		++i;
	}
	return S_OK;
}

bool SceneNode::VAddChild(std::shared_ptr<SceneNode> ikid) {
	if (!ikid) return false;

	SceneNodeList::iterator iter = std::find(m_children.begin(), m_children.end(), ikid);
	if (iter != m_children.end()) return false;

	ikid->m_pParent = shared_from_this();
	ikid->UpdateCumulativeTransform();
	m_children.push_back(ikid);

	UpdateMergedAABB();

	return true;
}

bool SceneNode::VRemoveChild(std::shared_ptr<SceneNode> cid) {
	if (!cid) return false;

	SceneNodeList::iterator iter = std::find(m_children.begin(), m_children.end(), cid);
	if (iter == m_children.end()) return false;

	cid->m_pParent.reset();
	cid->UpdateCumulativeTransform();
	m_children.erase(iter);

	UpdateMergedAABB();

	return true;
}

const SceneNodeList& SceneNode::VGetChildren() const {
	return m_children;
}

const SceneNodeProperties& SceneNode::Get() const {
	return m_props;
}

void SceneNode::UpdateMergedAABB() {
	DirectX::BoundingBox aabb_max = m_props.m_AABB_cumulative;
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		DirectX::BoundingBox::CreateMerged(aabb_max, aabb_max, (*i)->m_props.m_AABB_cumulative);
		++i;
	}
	++m_props.m_generation;
	m_props.m_AABB_merged = aabb_max;
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_merged, m_props.m_AABB_merged);

	std::shared_ptr<SceneNode> parent = m_pParent.lock();
	while (parent) {
		DirectX::BoundingBox::CreateMerged(aabb_max, parent->m_props.m_AABB_merged, aabb_max);
		parent->m_props.m_AABB_merged = aabb_max;
		DirectX::BoundingSphere::CreateFromBoundingBox(parent->m_props.m_sphere_merged, parent->m_props.m_AABB_merged);

		parent = parent->GetParent();
	}
}

void SceneNode::UpdateCumulativeTransform() {
	if (std::shared_ptr<SceneNode> pParent = m_pParent.lock()) {
		DirectX::XMStoreFloat4x4(
			&m_props.m_to_world_cumulative,
			DirectX::XMMatrixMultiply(
				DirectX::XMLoadFloat4x4(&pParent->m_props.m_to_world_cumulative),
				DirectX::XMLoadFloat4x4(&m_props.m_to_world)
			)
		);
		DirectX::XMStoreFloat4x4(&m_props.m_from_world_cumulative, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_props.m_to_world_cumulative)));
		DirectX::XMStoreFloat3(
			&m_props.m_scale_cumulative,
			DirectX::XMVectorMultiply(
				DirectX::XMLoadFloat3(&m_props.m_scale),
				DirectX::XMLoadFloat3(&pParent->m_props.m_scale_cumulative)
			)
		);
	}
	else {
		m_props.m_to_world_cumulative = m_props.m_to_world;
		m_props.m_from_world_cumulative = m_props.m_from_world;
		m_props.m_scale_cumulative = m_props.m_scale;
	}
	m_props.m_AABB.Transform(m_props.m_AABB_cumulative, m_props.FullCumulativeToWorld());
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_cumulative, m_props.m_AABB_cumulative);
	++m_props.m_generation;
	DirectX::BoundingBox aabb_max = m_props.m_AABB_cumulative;
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->UpdateCumulativeTransform();
		DirectX::BoundingBox::CreateMerged(aabb_max, aabb_max, (*i)->m_props.m_AABB_cumulative);
		++i;
	}
	m_props.m_AABB_merged = aabb_max;
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_merged, m_props.m_AABB_merged);
}

void SceneNode::UpdateCumulativeScale() {
	if (std::shared_ptr<SceneNode> pParent = m_pParent.lock()) {
		DirectX::XMStoreFloat3(
			&m_props.m_scale_cumulative,
			DirectX::XMVectorMultiply(
				DirectX::XMLoadFloat3(&m_props.m_scale),
				DirectX::XMLoadFloat3(&pParent->m_props.m_scale_cumulative)
			)
		);
	}
	else {
		m_props.m_scale_cumulative = m_props.m_scale;
	}
	m_props.m_AABB.Transform(m_props.m_AABB_cumulative, m_props.FullCumulativeToWorld());
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_cumulative, m_props.m_AABB_cumulative);
	++m_props.m_generation;
	DirectX::BoundingBox aabb_max = m_props.m_AABB_cumulative;
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->UpdateCumulativeScale();
		DirectX::BoundingBox::CreateMerged(aabb_max, aabb_max, (*i)->m_props.m_AABB_cumulative);
		++i;
	}
	m_props.m_AABB_merged = aabb_max;
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_merged, m_props.m_AABB_merged);
}

void SceneNode::SetTransform4x4(const DirectX::XMFLOAT4X4* to_world, const DirectX::XMFLOAT4X4* from_world) {
	DirectX::XMFLOAT4X4 to4x4;
	if (to_world == nullptr) {
		DirectX::XMStoreFloat4x4(&to4x4, DirectX::XMMatrixIdentity());
	}
	else {
		to4x4 = *to_world;
	}
	m_props.m_to_world = to4x4;

	if (!from_world) {
		DirectX::XMStoreFloat4x4(&m_props.m_from_world, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_props.m_to_world)));
	}
	else {
		m_props.m_from_world = *from_world;
	}

	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

void SceneNode::SetTransform(DirectX::FXMMATRIX to_world, DirectX::CXMMATRIX from_world, bool calulate_from) {
	DirectX::XMStoreFloat4x4(&m_props.m_to_world, to_world);
	if (calulate_from) {
		DirectX::XMStoreFloat4x4(&m_props.m_from_world, DirectX::XMMatrixInverse(nullptr, to_world));
	}
	else {
		DirectX::XMStoreFloat4x4(&m_props.m_from_world, from_world);
	}
	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

void SceneNode::SetParent(std::shared_ptr<SceneNode> parent_node) {
	std::shared_ptr<SceneNode> me = shared_from_this();
	if (std::shared_ptr<SceneNode> pParent = m_pParent.lock()) {
		pParent->VRemoveChild(me);
		m_pParent.reset();
	}
	if (parent_node) {
		parent_node->VAddChild(me);
	}
	else {
		m_pParent.reset();
	}
	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

std::shared_ptr<SceneNode> SceneNode::GetParent() {
	return m_pParent.lock();
}

void SceneNode::SetName(std::string name) {
	m_props.m_name = name;
}

void SceneNode::SetPosition(DirectX::XMVECTOR pos) {
	DirectX::XMFLOAT4 out{};
	DirectX::XMStoreFloat4(&out, pos);
	m_props.m_to_world.m[3][0] = out.x;
	m_props.m_to_world.m[3][1] = out.y;
	m_props.m_to_world.m[3][2] = out.z;
	m_props.m_to_world.m[3][3] = out.w;

	m_props.m_from_world.m[3][0] = -1.0f * out.x;
	m_props.m_from_world.m[3][1] = -1.0f * out.y;
	m_props.m_from_world.m[3][2] = -1.0f * out.z;
	m_props.m_from_world.m[3][3] = -1.0f * out.w;

	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

void SceneNode::SetPosition3(const DirectX::XMFLOAT3& pos) {
	m_props.m_to_world.m[3][0] = pos.x;
	m_props.m_to_world.m[3][1] = pos.y;
	m_props.m_to_world.m[3][2] = pos.z;
	m_props.m_to_world.m[3][3] = 1.0f;

	m_props.m_from_world.m[3][0] = -1.0f * pos.x;
	m_props.m_from_world.m[3][1] = -1.0f * pos.y;
	m_props.m_from_world.m[3][2] = -1.0f * pos.z;
	m_props.m_from_world.m[3][3] = 1.0f;

	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

void SceneNode::SetPosition4(const DirectX::XMFLOAT4& pos) {
	m_props.m_to_world.m[3][0] = pos.x;
	m_props.m_to_world.m[3][1] = pos.y;
	m_props.m_to_world.m[3][2] = pos.z;
	m_props.m_to_world.m[3][3] = pos.w;

	m_props.m_from_world.m[3][0] = -1.0f * pos.x;
	m_props.m_from_world.m[3][1] = -1.0f * pos.y;
	m_props.m_from_world.m[3][2] = -1.0f * pos.z;
	m_props.m_from_world.m[3][3] = -1.0f * pos.w;

	UpdateCumulativeTransform();
	UpdateMergedAABB();
}

void SceneNode::SetScale(const DirectX::XMFLOAT3& scale) {
	m_props.m_scale = scale;
	UpdateCumulativeScale();
}

void SceneNode::SetScale(DirectX::XMVECTOR scale) {
	DirectX::XMStoreFloat3(&m_props.m_scale, scale);
	UpdateCumulativeScale();
}

void SceneNode::SetDirtyFlags(uint32_t flags) {
	m_props.m_dirty_flags = flags;
}

void SceneNode::AddDirtyFlags(uint32_t flags) {
	m_props.m_dirty_flags |= flags;
}

void SceneNode::RemoveDirtyFlags(uint32_t flags) {
	m_props.m_dirty_flags &= ~flags;
}

void SceneNode::SetGroupID(uint32_t id) {
	m_props.m_group_id = id;
}

void SceneNode::SetAABB(const DirectX::BoundingBox& aabb) {
	m_props.m_AABB = aabb;
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere, m_props.m_AABB);
	m_props.m_AABB.Transform(m_props.m_AABB_cumulative, m_props.FullCumulativeToWorld());
	DirectX::BoundingSphere::CreateFromBoundingBox(m_props.m_sphere_cumulative, m_props.m_AABB_cumulative);
	UpdateMergedAABB();
}
