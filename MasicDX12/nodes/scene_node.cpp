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

SceneNode::~SceneNode() {}

void SceneNode::Accept(IVisitor& visitor) {
	visitor.Visit(*this);
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
	auto i = m_children.begin();
	auto end = m_children.end();
	while (i != end) {
		(*i)->VOnUpdate();
		++i;
	}
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

	return true;
}

bool SceneNode::VRemoveChild(std::shared_ptr<SceneNode> cid) {
	if (!cid) return false;

	SceneNodeList::iterator iter = std::find(m_children.begin(), m_children.end(), cid);
	if (iter == m_children.end()) return false;

	cid->m_pParent.reset();
	cid->UpdateCumulativeTransform();
	m_children.erase(iter);

	return true;
}

const SceneNodeProperties& SceneNode::Get() const {
	return m_props;
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
		DirectX::XMStoreFloat4x4(&m_props.m_from_world_cumulative, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_props.m_from_world_cumulative)));
		DirectX::XMStoreFloat3(
			&m_props.m_scale_cumulative,
			DirectX::XMVectorAdd(
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
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->UpdateCumulativeTransform();
		++i;
	}
}

void SceneNode::UpdateCumulativeScale() {
	if (std::shared_ptr<SceneNode> pParent = m_pParent.lock()) {
		DirectX::XMStoreFloat3(
			&m_props.m_scale_cumulative,
			DirectX::XMVectorAdd(
				DirectX::XMLoadFloat3(&m_props.m_scale),
				DirectX::XMLoadFloat3(&pParent->m_props.m_scale_cumulative)
			)
		);
	}
	else {
		m_props.m_scale_cumulative = m_props.m_scale;
	}
	SceneNodeList::iterator i = m_children.begin();
	SceneNodeList::iterator end = m_children.end();
	while (i != end) {
		(*i)->UpdateCumulativeScale();
		++i;
	}
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
}

std::shared_ptr<SceneNode> SceneNode::GetParent() {
	return m_pParent.lock();
}

void SceneNode::SetName(std::string name) {
	m_props.m_name = name;
}

void SceneNode::SetPosition3(const DirectX::XMFLOAT3& pos) {
	m_props.m_to_world.m[3][0] = pos.x;
	m_props.m_to_world.m[3][1] = pos.y;
	m_props.m_to_world.m[3][2] = pos.z;
	m_props.m_to_world.m[3][3] = 1.0f;

	m_props.m_from_world.m[3][0] = -1.0 * pos.x;
	m_props.m_from_world.m[3][1] = -1.0 * pos.y;
	m_props.m_from_world.m[3][2] = -1.0 * pos.z;
	m_props.m_from_world.m[3][3] = 1.0f;

	UpdateCumulativeTransform();
}

void SceneNode::SetScale(const DirectX::XMFLOAT3& scale) {
	m_props.m_scale = scale;
	UpdateCumulativeScale();
}

void SceneNode::SetScale(DirectX::XMVECTOR scale) {
	DirectX::XMStoreFloat3(&m_props.m_scale, scale);
	UpdateCumulativeScale();
}