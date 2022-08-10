#include "scene_node.h"
#include "scene.h"
#include "../engine/engine.h"
#include "../actors/transform_component.h"
#include "camera_node.h"

#include <algorithm>

SceneNode::SceneNode(const std::string& name, const DirectX::XMFLOAT4X4* to, const DirectX::XMFLOAT4X4* from) {
	SetTransform4x4(to, from);
	m_props.m_name = name;
	m_props.m_alpha_type = AlphaType::AlphaOpaque;
}

SceneNode::SceneNode(const std::string& name, DirectX::FXMMATRIX to, DirectX::CXMMATRIX from, bool calulate_from) {
	SetTransform(to, from, calulate_from);
	m_props.m_name = name;
	m_props.m_alpha_type = AlphaType::AlphaOpaque;
}

SceneNode::~SceneNode() {}

const SceneNodeProperties& SceneNode::VGet() const {
	return m_props;
}

void SceneNode::Accept(Visitor& visitor) {
	visitor.Visit(*this);
	for (auto& child : m_children) {
		child->Accept(visitor);
	}
}

void SceneNode::VSetTransform4x4(const DirectX::XMFLOAT4X4* to_world, const DirectX::XMFLOAT4X4* from_world) {
	SetTransform4x4(to_world, from_world);
}

void SceneNode::VSetTransform(DirectX::FXMMATRIX to_world, DirectX::CXMMATRIX from_world, bool calulate_from) {
	SetTransform(to_world, from_world, calulate_from);
}

DirectX::XMMATRIX SceneNode::VGetTransform() {
	return m_props.FromWorld();
}

DirectX::XMFLOAT4X4 SceneNode::VGetTransform4x4() {
	return m_props.FromWorld4x4();
}

DirectX::XMFLOAT4X4 SceneNode::VGetTransform4x4T() {
	return m_props.ToWorld4x4T();
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

bool SceneNode::VAddChild(std::shared_ptr<SceneNode> ikid) {
	if (!ikid) return false;

	SceneNodeList::iterator iter = std::find(m_children.begin(), m_children.end(), ikid);
	if (iter != m_children.end()) return false;

	ikid->m_pParent = shared_from_this();
	m_children.push_back(ikid);
	return true;
}

bool SceneNode::VRemoveChild(std::shared_ptr<SceneNode> cid) {
	if (!cid) return false;

	SceneNodeList::iterator iter = std::find(m_children.begin(), m_children.end(), cid);
	if (iter == m_children.end()) return false;

	cid->m_pParent.reset();
	m_children.erase(iter);

	return true;
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

std::shared_ptr<SceneNode> SceneNode::VGetParent() {
	return m_pParent.lock();
}

void SceneNode::SetAlpha(float alpha) {
	m_props.SetAlpha(alpha);
	for (SceneNodeList::const_iterator i = m_children.begin(); i != m_children.end(); ++i) {
		std::shared_ptr<SceneNode> scene_node = std::static_pointer_cast<SceneNode>(*i);
		scene_node->SetAlpha(alpha);
	}
}

float SceneNode::GetAlpha() const {
	return m_props.Alpha();
}

void SceneNode::SetName(std::string name) {
	m_props.m_name = name;
}

const std::string& SceneNode::GetName() const {
	return m_props.m_name;
}

DirectX::XMVECTOR SceneNode::GetPosition() const {
	return DirectX::XMVectorSet(m_props.m_to_world.m[3][0], m_props.m_to_world.m[3][1], m_props.m_to_world.m[3][2], 1.0f);
}

DirectX::XMFLOAT3 SceneNode::GetPosition3() const {
	return DirectX::XMFLOAT3(m_props.m_to_world.m[3][0], m_props.m_to_world.m[3][1], m_props.m_to_world.m[3][2]);
}

DirectX::XMFLOAT4 SceneNode::GetPosition4() const {
	return DirectX::XMFLOAT4(m_props.m_to_world.m[3][0], m_props.m_to_world.m[3][1], m_props.m_to_world.m[3][2], 1.0f);
}

void SceneNode::SetPosition3(const DirectX::XMFLOAT3& pos) {
	m_props.m_to_world.m[3][0] = pos.x;
	m_props.m_to_world.m[3][1] = pos.y;
	m_props.m_to_world.m[3][2] = pos.z;
	m_props.m_to_world.m[3][3] = 1.0f;
}

DirectX::XMFLOAT3 SceneNode::GetWorldPosition3() const {
	DirectX::XMFLOAT3 pos = GetPosition3();
	if (std::shared_ptr<SceneNode> pParent = m_pParent.lock()) {
		while (pParent) {
			DirectX::XMFLOAT3 wp1 = pParent->GetWorldPosition3();
			pos.x += wp1.x;
			pos.y += wp1.y;
			pos.z += wp1.z;

			pParent = pParent->m_pParent.lock();
		}
	}
	return pos;
}

DirectX::XMVECTOR SceneNode::GetWorldPosition() const {
	DirectX::XMFLOAT3 res = GetWorldPosition3();
	return DirectX::XMLoadFloat3(&res);
}

DirectX::XMVECTOR SceneNode::GetDirection() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_props.m_to_world;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return DirectX::XMVector4Transform(forward, just_rot);
}

DirectX::XMFLOAT3 SceneNode::GetDirection3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, GetDirection());

	return result;
}

DirectX::XMVECTOR SceneNode::GetUp() const {
	DirectX::XMFLOAT4X4 just_rot4x4 = m_props.m_to_world;
	just_rot4x4.m[3][0] = 0.0f;
	just_rot4x4.m[3][1] = 0.0f;
	just_rot4x4.m[3][2] = 0.0f;
	just_rot4x4.m[3][3] = 1.0f;
	DirectX::XMMATRIX just_rot = DirectX::XMLoadFloat4x4(&just_rot4x4);

	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	return DirectX::XMVector4Transform(up, just_rot);
}

DirectX::XMFLOAT3 SceneNode::GetUp3f() const {
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, GetUp());

	return result;
}

const DirectX::XMFLOAT3& SceneNode::GetScale3f() const {
	return m_props.m_scale;
}

DirectX::XMVECTOR SceneNode::GetScale() const {
	return DirectX::XMLoadFloat3(&m_props.m_scale);
}

void SceneNode::SetScale(const DirectX::XMFLOAT3& scale) {
	m_props.m_scale = scale;
}

void SceneNode::SetScale(DirectX::XMVECTOR scale) {
	DirectX::XMStoreFloat3(&m_props.m_scale, scale);
}

void SceneNode::SetMaterial(const Material& mat) {
	m_props.m_material = mat;
}

void SceneNode::SetTransform(DirectX::FXMMATRIX to_world, DirectX::CXMMATRIX from_world, bool calulate_from) {
	DirectX::XMStoreFloat4x4(&m_props.m_to_world, to_world);
	if (calulate_from) {
		DirectX::XMStoreFloat4x4(&m_props.m_from_world, DirectX::XMMatrixInverse(nullptr, to_world));
	}
	else {
		DirectX::XMStoreFloat4x4(&m_props.m_from_world, from_world);
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
}