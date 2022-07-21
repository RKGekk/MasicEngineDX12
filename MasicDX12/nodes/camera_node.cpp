#include "camera_node.h"
#include "scene.h"
#include "../engine/engine.h"
#include "../actors/orientation_relation_component.h"
#include "../actors/transform_component.h"
#include "../tools/memory_utility.h"

CameraNode::CameraNode(const DirectX::XMFLOAT4X4& t, const Frustum& frustum) : SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_0, &t), m_Frustum(frustum), m_bActive(true), m_DebugCamera(false), m_pTarget(std::shared_ptr<SceneNode>()) {
	m_CamOffsetVector = { 0.0f, 0.0f, -50.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
}

CameraNode::CameraNode(DirectX::FXMMATRIX t, const Frustum& frustum) : SceneNode(WeakBaseRenderComponentPtr(), RenderPass::RenderPass_0, t, DirectX::XMMatrixIdentity(), true), m_Frustum(frustum), m_bActive(true), m_DebugCamera(false), m_pTarget(std::shared_ptr<SceneNode>()) {
	m_CamOffsetVector = { 0.0f, 0.0f, -50.0f, 0.0f };
	DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
}

HRESULT CameraNode::VRender(Scene* pScene) {
	if (m_DebugCamera) {
		pScene->PopMatrix();
		m_Frustum.Render();
		pScene->PushAndSetMatrix4x4(m_Props.ToWorld4x4());
	}

	return S_OK;
}

HRESULT CameraNode::VOnRestore(Scene* pScene) {
	m_Frustum.SetAspect(((float)Engine::GetEngine()->GetConfig().m_screen_width) / ((float)Engine::GetEngine()->GetConfig().m_screen_height));
	DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixPerspectiveFovLH(m_Frustum.m_Fov, m_Frustum.m_Aspect, m_Frustum.m_Near, m_Frustum.m_Far));
	return S_OK;
}

bool CameraNode::VIsVisible(Scene* pScene) const {
	return m_bActive;
}

Frustum& CameraNode::GetFrustum() {
	return m_Frustum;
}

void CameraNode::SetTarget(std::shared_ptr<SceneNode> pTarget) {
	m_pTarget = pTarget;
}

void CameraNode::ClearTarget() {
	m_pTarget = std::shared_ptr<SceneNode>();
}

std::shared_ptr<SceneNode> CameraNode::GetTarget() {
	return m_pTarget;
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(Scene* pScene) {
	DirectX::XMMATRIX world = pScene->GetTopMatrix();
	DirectX::XMMATRIX view = VGet().FromWorld();
	DirectX::XMMATRIX worldView = DirectX::XMMatrixMultiply(world, view);
	return DirectX::XMMatrixMultiply(worldView, DirectX::XMLoadFloat4x4(&m_Projection));
}

DirectX::XMMATRIX CameraNode::GetWorldViewProjection(DirectX::XMMATRIX world) {
	DirectX::XMMATRIX view = VGet().FromWorld();
	DirectX::XMMATRIX worldView = DirectX::XMMatrixMultiply(world, view);
	return DirectX::XMMatrixMultiply(worldView, DirectX::XMLoadFloat4x4(&m_Projection));
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4(Scene* pScene) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, GetWorldViewProjection(pScene));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(Scene* pScene) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(GetWorldViewProjection(pScene)));
	return res;
}

DirectX::XMFLOAT4X4 CameraNode::GetWorldViewProjection4x4T(DirectX::XMMATRIX world) {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(world));
	return res;
}

DirectX::XMMATRIX CameraNode::GetViewProjection() {
	DirectX::XMMATRIX view = VGet().FromWorld();
	return DirectX::XMMatrixMultiply(view, DirectX::XMLoadFloat4x4(&m_Projection));
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

HRESULT CameraNode::SetViewTransform(Scene* pScene) {
	using namespace DirectX;
	if (m_pTarget) {
		ActorId act_id = m_pTarget->VFindMyActor();
		std::shared_ptr<Actor> act = MakeStrongPtr(Engine::GetEngine()->GetGameLogic()->VGetActor(act_id));
		std::shared_ptr<OrientationRelationComponent> oc;
		std::shared_ptr<TransformComponent> tc;
		if (act) {
			oc = MakeStrongPtr(act->GetComponent<OrientationRelationComponent>(ActorComponent::GetIdFromName("OrientationRelationComponent")));
			tc = MakeStrongPtr(act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")));
		}
		if (oc) {
			const EngineOptions& options = Engine::GetEngine()->GetConfig();

			XMFLOAT3 p = tc->GetPosition3f();
			float tt = Engine::GetEngine()->GetTimer().TotalTime();
			XMVECTOR at_xm = oc->VGetAt() * options.m_game_cam_offset_z + oc->VGetUp() * options.m_game_cam_offset_y + oc->VGetRight() * options.m_game_cam_offset_x;
			XMFLOAT3 at;
			XMStoreFloat3(&at, at_xm);
			p.x += at.x;
			p.y += at.y;
			p.z += at.z;
			XMMATRIX orient = XMMatrixMultiply(oc->VGetOrient(), XMMatrixRotationAxis(oc->VGetRight(), XMConvertToRadians(options.m_game_cam_rotate_x)));
			XMMATRIX translation = XMMatrixTranslation(p.x, p.y, p.z);
			XMMATRIX transform = XMMatrixMultiply(
				orient,
				translation
			);
			VSetTransform(transform, DirectX::XMMatrixIdentity(), true);
		}
		else {
			DirectX::XMMATRIX mat = m_pTarget->VGet().ToWorld();
			DirectX::XMVECTOR at = DirectX::XMLoadFloat4(&m_CamOffsetVector);
			DirectX::XMVECTOR atWorld = DirectX::XMVector4Transform(at, mat);
			VSetTransform(mat, DirectX::XMMatrixIdentity(), true);
		}
	}

	DirectX::XMStoreFloat4x4(&m_View, VGet().FromWorld());

	return S_OK;
}

DirectX::XMMATRIX CameraNode::GetProjection() {
	return DirectX::XMLoadFloat4x4(&m_Projection);
}

const DirectX::XMFLOAT4X4& CameraNode::GetProjection4x4f() {
	return m_Projection;
}

DirectX::XMFLOAT4X4 CameraNode::GetProjection4x4fT() {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_Projection));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}


DirectX::XMMATRIX CameraNode::GetView() {
	return DirectX::XMLoadFloat4x4(&m_View);
}

const DirectX::XMFLOAT4X4& CameraNode::GetView4x4() {
	return m_View;
}

DirectX::XMFLOAT4X4 CameraNode::GetView4x4T() {
	DirectX::XMMATRIX t = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_View));
	DirectX::XMFLOAT4X4 t4x4;
	DirectX::XMStoreFloat4x4(&t4x4, t);
	return t4x4;
}

DirectX::XMFLOAT3 CameraNode::GetViewPos3() {
	return DirectX::XMFLOAT3(m_View.m[3][0], m_View.m[3][1], m_View.m[3][2]);
}

DirectX::XMVECTOR CameraNode::GetViewPos() {
	DirectX::XMFLOAT3 pos = GetViewPos3();
	return DirectX::XMLoadFloat3(&pos);
}

void CameraNode::SetCameraOffset(const DirectX::XMFLOAT4& cameraOffset) {
	m_CamOffsetVector = cameraOffset;
}