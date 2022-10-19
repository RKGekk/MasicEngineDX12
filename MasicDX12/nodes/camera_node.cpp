#include "camera_node.h"
#include "scene.h"
#include "../application.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

CameraNode::CameraNode(const std::string& name) : SceneNode(name) {
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixIdentity());
}

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform) : SceneNode(name, &camera_transform) {
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixIdentity());
}

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj) : SceneNode(name, &camera_transform) {
	m_projection = proj;
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true) {
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixIdentity());
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true) {
	DirectX::XMStoreFloat4x4(&m_projection, proj);
}

HRESULT CameraNode::VOnRestore() {
	SceneNode::VOnRestore();
	return S_OK;
}

HRESULT CameraNode::VOnUpdate() {
	return SceneNode::VOnUpdate();
}

void CameraNode::SetProjection(DirectX::FXMMATRIX proj) {
	DirectX::XMStoreFloat4x4(&m_projection, proj);
}

void CameraNode::SetProjection(const DirectX::XMFLOAT4X4& proj) {
	m_projection = proj;
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(DirectX::FXMMATRIX world) const {
	DirectX::XMMATRIX view = Get().CumulativeFromWorld();
	DirectX::XMMATRIX world_view = DirectX::XMMatrixMultiply(world, view);
	return DirectX::XMMatrixMultiply(world_view, DirectX::XMLoadFloat4x4(&m_projection));
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(const DirectX::XMFLOAT4X4& world) const {
	DirectX::XMMATRIX world_xm = DirectX::XMLoadFloat4x4(&world);
	return GetWorldViewProjection(world_xm);
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4(DirectX::XMMATRIX world) const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetWorldViewProjection(world));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(DirectX::XMMATRIX world) const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetWorldViewProjection(world)));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4(const DirectX::XMFLOAT4X4& world) const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetWorldViewProjection(world));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(const DirectX::XMFLOAT4X4& world) const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetWorldViewProjection(world)));
	return res;
}

DirectX::XMMATRIX CameraNode::GetViewProjection() const {
	DirectX::XMMATRIX view = Get().CumulativeFromWorld();
	return DirectX::XMMatrixMultiply(view, DirectX::XMLoadFloat4x4(&m_projection));
}

DirectX::XMFLOAT4X4 CameraNode::GetViewProjection4x4() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetViewProjection());
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetViewProjection4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetViewProjection()));
	return res;
}

DirectX::XMMATRIX CameraNode::GetProjection() const {
	return DirectX::XMLoadFloat4x4(&m_projection);
}

const DirectX::XMFLOAT4X4& CameraNode::GetProjection4x4f() const {
	return m_projection;
}

DirectX::XMFLOAT4X4 CameraNode::GetProjection4x4fT() const {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_projection));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMMATRIX CameraNode::GetView() const {
	return Get().CumulativeFromWorld();
}

const DirectX::XMFLOAT4X4& CameraNode::GetView4x4f() const {
	return Get().CumulativeFromWorld4x4();
}

DirectX::XMFLOAT4X4 CameraNode::GetView4x4fT() const {
	return Get().CumulativeFromWorld4x4T();
}