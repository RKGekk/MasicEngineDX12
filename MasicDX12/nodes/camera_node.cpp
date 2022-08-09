#include "camera_node.h"
#include "scene.h"
#include "../application.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& view, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, &view), m_fovy(fovy), m_aspect(aspect) {
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	m_frustum.Origin = DirectX::XMFLOAT3(view._41, view._42, view._43);
}

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& proj) : SceneNode(name, &view) {
	m_projection = proj;
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	m_frustum.Origin = DirectX::XMFLOAT3(view._41, view._42, view._43);
	m_fovy = 2.0f * atanf(1.0f / m_projection.m[1][1]);
	m_aspect = m_projection.m[1][1] / m_projection.m[0][0];
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX view, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, view, DirectX::XMMatrixIdentity(), true), m_fovy(fovy), m_aspect(aspect) {
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	const DirectX::XMFLOAT4X4& view_xm = m_props.FromWorld4x4();
	m_frustum.Origin = DirectX::XMFLOAT3(view_xm._41, view_xm._42, view_xm._43);
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX view, DirectX::CXMMATRIX proj) : SceneNode(name, view, DirectX::XMMatrixIdentity(), true) {
	DirectX::XMStoreFloat4x4(&m_projection, proj);
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	const DirectX::XMFLOAT4X4& view_xm = m_props.FromWorld4x4();
	m_frustum.Origin = DirectX::XMFLOAT3(view_xm._41, view_xm._42, view_xm._43);
	m_fovy = 2.0f * atanf(1.0f / m_projection.m[1][1]);
	m_aspect = m_projection.m[1][1] / m_projection.m[0][0];
}

HRESULT CameraNode::VOnRestore() {
	SceneNode::VOnRestore();
	float new_aspect = Application::Get().GetApplicationOptions().GetAspect();
	if(m_aspect == new_aspect) return S_OK;

	m_aspect = new_aspect;
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	const DirectX::XMFLOAT4X4& view_xm = m_props.FromWorld4x4();
	m_frustum.Origin = DirectX::XMFLOAT3(view_xm._41, view_xm._42, view_xm._43);
	return S_OK;
}

const DirectX::BoundingFrustum& CameraNode::GetFrustum() const {
	return m_frustum;
}

void CameraNode::SetFovYRad(float fovy) {
	if(m_fovy == fovy) return;
	m_fovy = fovy;
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	const DirectX::XMFLOAT4X4& view_xm = m_props.FromWorld4x4();
	m_frustum.Origin = DirectX::XMFLOAT3(view_xm._41, view_xm._42, view_xm._43);
}

void CameraNode::SetFovYDeg(float fovy) {
	SetFovYRad(DirectX::XMConvertToRadians(fovy));
}

float CameraNode::GetFovYRad() {
	return atanf(m_frustum.TopSlope / m_frustum.Near);
}

float CameraNode::GetFovYDeg() {
	return DirectX::XMConvertToDegrees(GetFovYRad());
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(DirectX::FXMMATRIX world) {
	DirectX::XMMATRIX view = VGet().FromWorld();
	DirectX::XMMATRIX world_view = DirectX::XMMatrixMultiply(world, view);
	return DirectX::XMMatrixMultiply(world_view, DirectX::XMLoadFloat4x4(&m_projection));
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(const DirectX::XMFLOAT4X4& world) {
	DirectX::XMMATRIX world_xm = DirectX::XMLoadFloat4x4(&world);
	return GetWorldViewProjection(world_xm);
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4(DirectX::XMMATRIX world) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetWorldViewProjection(world));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(DirectX::XMMATRIX world) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetWorldViewProjection(world)));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4(const DirectX::XMFLOAT4X4& world) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetWorldViewProjection(world));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(const DirectX::XMFLOAT4X4& world) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetWorldViewProjection(world)));
	return res;
}

DirectX::XMMATRIX CameraNode::GetViewProjection() {
	DirectX::XMMATRIX view = VGet().FromWorld();
	return DirectX::XMMatrixMultiply(view, DirectX::XMLoadFloat4x4(&m_projection));
}

DirectX::XMFLOAT4X4 CameraNode::GetViewProjection4x4() {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetViewProjection());
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetViewProjection4x4T() {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetViewProjection()));
	return res;
}

DirectX::XMMATRIX CameraNode::GetProjection() {
	return DirectX::XMLoadFloat4x4(&m_projection);
}

const DirectX::XMFLOAT4X4& CameraNode::GetProjection4x4f() {
	return m_projection;
}

DirectX::XMFLOAT4X4 CameraNode::GetProjection4x4fT() {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_projection));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}