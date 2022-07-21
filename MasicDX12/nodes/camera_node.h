#pragma once

#include <memory>

#include <DirectXMath.h>

#include "scene_node.h"
#include "frustum.h"
#include "../actors/actor.h"
#include "render_pass.h"

class CameraNode : public SceneNode {
public:
	CameraNode(const DirectX::XMFLOAT4X4& t, const Frustum& frustum);
	CameraNode(DirectX::FXMMATRIX t, const Frustum& frustum);

	virtual HRESULT VRender(Scene* pScene) override;
	virtual HRESULT VOnRestore(Scene* pScene) override;
	virtual bool VIsVisible(Scene* pScene) const override;

	Frustum& GetFrustum();
	void SetTarget(std::shared_ptr<SceneNode> pTarget);
	void ClearTarget();
	std::shared_ptr<SceneNode> GetTarget();

	DirectX::XMMATRIX GetWorldViewProjection(Scene* pScene);
	DirectX::XMMATRIX GetWorldViewProjection(DirectX::XMMATRIX world);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4(Scene* pScene);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(Scene* pScene);
	DirectX::XMFLOAT4X4 GetWorldViewProjection4x4T(DirectX::XMMATRIX world);
	DirectX::XMMATRIX GetViewProjection();
	DirectX::XMFLOAT4X4 GetViewProjection4x4();
	DirectX::XMFLOAT4X4 GetViewProjection4x4T();
	HRESULT SetViewTransform(Scene* pScene);

	DirectX::XMMATRIX GetProjection();
	const DirectX::XMFLOAT4X4& GetProjection4x4f();
	DirectX::XMFLOAT4X4 GetProjection4x4fT();
	DirectX::XMMATRIX GetView();
	const DirectX::XMFLOAT4X4& GetView4x4();
	DirectX::XMFLOAT4X4 GetView4x4T();
	DirectX::XMFLOAT3 GetViewPos3();
	DirectX::XMVECTOR GetViewPos();

	void SetCameraOffset(const DirectX::XMFLOAT4& cameraOffset);

protected:
	Frustum m_Frustum;
	DirectX::XMFLOAT4X4 m_Projection;
	DirectX::XMFLOAT4X4 m_View;
	bool m_bActive;
	bool m_DebugCamera;
	std::shared_ptr<SceneNode> m_pTarget;
	DirectX::XMFLOAT4 m_CamOffsetVector;
};