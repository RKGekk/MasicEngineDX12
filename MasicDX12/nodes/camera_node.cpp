#include "camera_node.h"
#include "scene.h"
#include "../application.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, &camera_transform), m_fovy(fovy), m_aspect(aspect) {
	SetData(DirectX::XMLoadFloat4x4(&camera_transform), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

CameraNode::CameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj) : SceneNode(name, &camera_transform) {
	SetData(DirectX::XMLoadFloat4x4(&camera_transform), DirectX::XMLoadFloat4x4(&proj));
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, float fovy, float aspect, float near_clip, float far_clip) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true), m_fovy(fovy), m_aspect(aspect) {
	SetData(camera_transform, DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

CameraNode::CameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) : SceneNode(name, camera_transform, DirectX::XMMatrixIdentity(), true) {
	SetData(camera_transform, proj);
}

HRESULT CameraNode::VOnRestore() {
	float new_aspect = Application::Get().GetApplicationOptions().GetAspect();
	if(m_aspect == new_aspect) return S_OK;

	m_aspect = new_aspect;
	SetData(m_props.ToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));

	SceneNode::VOnRestore();

	return S_OK;
}

const DirectX::BoundingFrustum& CameraNode::GetFrustum() const {
	return m_frustum;
}

void CameraNode::SetFovYRad(float fovy) {
	if(m_fovy == fovy) return;

	m_fovy = fovy;
	SetData(m_props.ToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
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

DirectX::XMMATRIX CameraNode::GetView() {
	return VGet().FromWorld();
}

const DirectX::XMFLOAT4X4& CameraNode::GetView4x4f() {
	return VGet().FromWorld4x4();
}

DirectX::XMFLOAT4X4 CameraNode::GetView4x4fT() {
	return VGet().FromWorld4x4T();
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
