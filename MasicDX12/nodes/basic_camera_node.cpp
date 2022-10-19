#include "basic_camera_node.h"

#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"
#include "../application.h"

BasicCameraNode::BasicCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, float fovy, float aspect, float near_clip, float far_clip) : CameraNode(name, camera_transform), m_fovy(fovy), m_aspect(aspect) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

BasicCameraNode::BasicCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const DirectX::XMFLOAT4X4& proj) : CameraNode(name, camera_transform) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMLoadFloat4x4(&proj));
}

BasicCameraNode::BasicCameraNode(const std::string& name, const DirectX::BoundingFrustum& frustum) : CameraNode(name) {
	SetData(frustum);
}

BasicCameraNode::BasicCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, float fovy, float aspect, float near_clip, float far_clip) : CameraNode(name, camera_transform), m_fovy(fovy), m_aspect(aspect) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

BasicCameraNode::BasicCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) : CameraNode(name, camera_transform) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), proj);
}

HRESULT BasicCameraNode::VOnRestore() {
	float new_aspect = Application::Get().GetApplicationOptions().GetAspect();
	if (m_aspect == new_aspect) return S_OK;

	m_aspect = new_aspect;
	SetData(m_props.CumulativeToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));

	CameraNode::VOnRestore();

	return S_OK;
}

HRESULT BasicCameraNode::VOnUpdate() {

	uint32_t dirty_flags = Get().GetDirtyFlags();
	constexpr uint32_t camera_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Camera);
	constexpr uint32_t transform_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Transform);

	if ((dirty_flags & camera_flag) || (dirty_flags & transform_flag)) {
		UpdateFrustum();
	}

	return CameraNode::VOnUpdate();
}

const DirectX::BoundingFrustum& BasicCameraNode::GetFrustum() const {
	return m_frustum;
}

void BasicCameraNode::UpdateFrustum() {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMLoadFloat4x4(&m_projection));
}

void BasicCameraNode::SetFovYRad(float fovy) {
	if (m_fovy == fovy) return;

	m_fovy = fovy;
	SetData(m_props.CumulativeToWorld(), DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
}

void BasicCameraNode::SetFovYDeg(float fovy) {
	SetFovYRad(DirectX::XMConvertToRadians(fovy));
}

float BasicCameraNode::GetFovYRad() const {
	return m_fovy;
}

float BasicCameraNode::GetFovYDeg() const {
	return DirectX::XMConvertToDegrees(m_fovy);
}

void BasicCameraNode::SetNear(float near_cut) {
	if (m_frustum.Near == near_cut) return;

	m_frustum.Near = near_cut;
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
}

void BasicCameraNode::SetFar(float far_cut) {
	if (m_frustum.Far == far_cut) return;

	m_frustum.Far = far_cut;
	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, m_frustum.Near, m_frustum.Far));
}

void BasicCameraNode::SetProjection(const DirectX::BoundingFrustum& frustum) {
	SetData(frustum);
}

void BasicCameraNode::SetProjection(float fovy, float aspect, float near_clip, float far_clip) {
	SetData(DirectX::XMLoadFloat4x4(&Get().CumulativeToWorld4x4()), DirectX::XMMatrixPerspectiveFovLH(fovy, aspect, near_clip, far_clip));
}

void BasicCameraNode::SetData(DirectX::FXMMATRIX camera_transform, DirectX::CXMMATRIX proj) {
	DirectX::XMStoreFloat4x4(&m_projection, proj);
	m_frustum = DirectX::BoundingFrustum(DirectX::XMLoadFloat4x4(&m_projection));
	DirectX::XMFLOAT4X4 camera_transform_xm;
	DirectX::XMStoreFloat4x4(&camera_transform_xm, camera_transform);
	m_frustum.Origin = DirectX::XMFLOAT3(camera_transform_xm._41, camera_transform_xm._42, camera_transform_xm._43);
	DirectX::XMStoreFloat4(&m_frustum.Orientation, DirectX::XMQuaternionRotationMatrix(camera_transform));
	m_fovy = 2.0f * atanf(1.0f / m_projection.m[1][1]);
	m_aspect = m_projection.m[1][1] / m_projection.m[0][0];
}

void BasicCameraNode::SetData(DirectX::BoundingFrustum frustum) {
	DirectX::XMMATRIX view_rot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&frustum.Orientation));
	SetTransform(view_rot, DirectX::XMMatrixIdentity(), true);
	SetPosition3(frustum.Origin);

	m_fovy = 2.0f * atanf(frustum.TopSlope);
	m_aspect = frustum.TopSlope / frustum.RightSlope;

	DirectX::XMStoreFloat4x4(&m_projection, DirectX::XMMatrixPerspectiveFovLH(m_fovy, m_aspect, frustum.Near, frustum.Far));
	m_frustum = frustum;
}
