#include "camera_node.h"
#include "scene.h"
#include "../application.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, &camera_transform), m_fovy(fovy), m_aspect(aspect) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj) : SceneNode(name, &camera_transform) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMLoadFloat4x4(&proj));
}

CameraNode::CameraNode(const std::string& name, const DirectX::BoundingFrustum& frustum) : SceneNode(name, DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), false) {
	SetData(frustum);
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true), m_fovy(fovy), m_aspect(aspect) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), proj);
}

HRESULT CameraNode::VOnRestore() {
	float new_aspect = Application::Get().GetApplicationOptions().GetAspect();
	if(m_aspect == new_aspect) return S_OK;

	m_aspect = new_aspect;
	SetData(m_props.CumulativeToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));

	SceneNode::VOnRestore();

	return S_OK;
}

const DirectX::BoundingFrustum& CameraNode::GetFrustum() const {
	return m_frustum;
}

void CameraNode::SetFovYRad(float fovy) {
	if (m_fovy == fovy) return;

	m_fovy = fovy;
	SetData(m_props.CumulativeToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
}

void CameraNode::SetFovYDeg(float fovy) {
	SetFovYRad(DirectX::XMConvertToRadians(fovy));
}

float CameraNode::GetFovYRad() const {
	return m_fovy;
}

float CameraNode::GetFovYDeg() const {
	return DirectX::XMConvertToDegrees(m_fovy);
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

void CameraNode::SetData(DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) {
	DirectX::XMStoreFloat4x4(&m_projection, proj);
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	DirectX::XMFLOAT4X4 camera_transform_xm;
	DirectX::XMStoreFloat4x4(&camera_transform_xm, camera_transform);
	m_frustum.Origin = DirectX::XMFLOAT3(camera_transform_xm._41, camera_transform_xm._42, camera_transform_xm._43);
	DirectX::XMStoreFloat4(&m_frustum.Orientation, DirectX::XMQuaternionRotationMatrix(camera_transform));
	m_fovy = 2.0f * atanf(1.0f / m_projection.m[1][1]);
	m_aspect = m_projection.m[1][1] / m_projection.m[0][0];
}

void CameraNode::SetData(DirectX::BoundingFrustum frustum) {
	DirectX::XMMATRIX view_rot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&frustum.Orientation));
	SetTransform(view_rot, DirectX::XMMatrixIdentity(), true);
	SetPosition3(frustum.Origin);

	m_fovy = 2.0f * atanf(frustum.TopSlope);
	m_aspect = frustum.TopSlope / frustum.RightSlope;

	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, frustum.Near, frustum.Far));
	m_frustum = frustum;
}
