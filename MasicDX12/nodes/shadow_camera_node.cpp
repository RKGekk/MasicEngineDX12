#include "shadow_camera_node.h"

#include "scene.h"
#include "../application.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

ShadowCameraNode::ShadowCameraNode(const std::string& name, const DirectX::XMFLOAT4X4& camera_transform, const std::shared_ptr<SceneNode> root_node, const std::shared_ptr<LightNode> light_node, ShadowCameraProps camera_props) : CameraNode(name, camera_transform), m_shadow_camera_props(camera_props), m_root_node(root_node), m_light_node(light_node) {
	UpdateShadowTransform();
}

ShadowCameraNode::ShadowCameraNode(const std::string& name, DirectX::FXMMATRIX camera_transform, const std::shared_ptr<SceneNode> root_node, const std::shared_ptr<LightNode> light_node, ShadowCameraProps camera_props) : CameraNode(name, camera_transform), m_shadow_camera_props(camera_props), m_root_node(root_node), m_light_node(light_node) {
	UpdateShadowTransform();
}

HRESULT ShadowCameraNode::VOnRestore() {
	CameraNode::VOnRestore();

	return S_OK;
}

HRESULT ShadowCameraNode::VOnUpdate() {

    uint32_t dirty_flags = Get().GetDirtyFlags();
    constexpr uint32_t camera_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Camera);
    constexpr uint32_t transform_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Transform);

    if ((dirty_flags & camera_flag) || (dirty_flags & transform_flag)) {
        UpdateShadowTransform();
    }

	return CameraNode::VOnUpdate();
}

const DirectX::BoundingOrientedBox& ShadowCameraNode::GetFrustum() const {
	return m_frustum;
}

void ShadowCameraNode::UpdateShadowTransform() {
    using namespace DirectX;

    std::shared_ptr<SceneNode> root_node = m_root_node.lock();
    std::shared_ptr<LightNode> light_node = m_light_node.lock();

    BoundingSphere scene_radius_sphere = root_node->Get().MergedSphere();
    float scene_bounds_radius = scene_radius_sphere.Radius;
    XMFLOAT3 scene_bounds_center = scene_radius_sphere.Center;

    XMVECTOR target_pos = XMLoadFloat3(&scene_bounds_center);
    XMMATRIX view = Get().CumulativeFromWorld();

    // Transform bounding sphere to light space.
    XMFLOAT3 sphere_center_vs;
    XMStoreFloat3(&sphere_center_vs, XMVector3TransformCoord(target_pos, view));

    // Ortho frustum in light space encloses scene.
    float l = sphere_center_vs.x - scene_bounds_radius;
    float r = sphere_center_vs.x + scene_bounds_radius;
    float b = sphere_center_vs.y - scene_bounds_radius;
    float t = sphere_center_vs.y + scene_bounds_radius;
    float n = sphere_center_vs.z - scene_bounds_radius;
    float f = sphere_center_vs.z + scene_bounds_radius;
    XMMATRIX projection = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX transform_to_uv (
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f
    );

    XMMATRIX S = view * projection * transform_to_uv;

    XMStoreFloat4x4(&m_projection, projection);
    XMStoreFloat4x4(&m_shadow_transform, S);

    BoundingBox scene_aabb = root_node->Get().MergedAABB();
    //m_frustum.Extents = XMFLOAT3(scene_bounds_radius, scene_bounds_radius, scene_bounds_radius);
    m_frustum.Extents = scene_aabb.Extents;
    m_frustum.Center = scene_bounds_center;
    XMStoreFloat4(&m_frustum.Orientation, XMQuaternionRotationMatrix(Get().CumulativeToWorld()));
}

void ShadowCameraNode::SetProjection(const DirectX::BoundingOrientedBox& frustum) {
    DirectX::XMMATRIX view_rot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&frustum.Orientation));
    DirectX::XMFLOAT3 ext = {};
    DirectX::XMStoreFloat3(&ext, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&frustum.Extents), view_rot));
    float l = frustum.Center.x - ext.x;
    float r = frustum.Center.x + ext.x;
    float b = frustum.Center.y - ext.y;
    float t = frustum.Center.y + ext.y;
    float n = frustum.Center.z - ext.z;
    float f = frustum.Center.z + ext.z;
    DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
    XMStoreFloat4x4(&m_projection, projection);
}

DirectX::XMMATRIX ShadowCameraNode::GetShadowTranform() const {
    return DirectX::XMLoadFloat4x4(&m_shadow_transform);
}

const DirectX::XMFLOAT4X4& ShadowCameraNode::GetShadowTranform4x4() const {
    return m_shadow_transform;
}

DirectX::XMFLOAT4X4 ShadowCameraNode::GetShadowTranform4x4T() const {
    DirectX::XMFLOAT4X4 res;
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetShadowTranform()));
    return res;
}

const ShadowCameraNode::ShadowCameraProps& ShadowCameraNode::GetShadowProps() const {
    return m_shadow_camera_props;
}
