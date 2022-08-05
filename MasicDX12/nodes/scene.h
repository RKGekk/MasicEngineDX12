#pragma once

#include <memory>

#include "i_scene_node.h"
#include "scene_node.h"
#include "../graphics/i_renderer.h"
#include "matrix_stack.h"
#include "alpha_scene_node.h"
#include "../events/i_event_data.h"

class CameraNode;
class SkyNode;
class LightNode;
class LightManager;
class ShadowManager;
class RootNode;

class Scene {
protected:
	bool m_scene_active = true;

	std::shared_ptr<RootNode> m_Root;
	std::shared_ptr<CameraNode> m_Camera;
	IRenderer* m_Renderer;

	std::shared_ptr<MatrixStack> m_MatrixStack;
	AlphaSceneNodes m_AlphaSceneNodes;
	SceneActorMap m_ActorMap;
	ActorComponentMap m_ActorComponentMap;

	std::unique_ptr<LightManager> m_LightManager;
	std::unique_ptr<ShadowManager> m_ShadowManager;

	void RenderAlphaPass();

public:
	Scene(IRenderer* renderer);
	virtual ~Scene();

	HRESULT OnRender();
	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate(float deltaSeconds);
	std::shared_ptr<ISceneNode> FindActor(ActorId aid, ComponentId cid);
	bool AddChild(ActorId aid, ComponentId cid, std::shared_ptr<ISceneNode> kid);
	bool RemoveChild(ActorId aid, ComponentId cid);

	void NewRenderComponentDelegate(IEventDataPtr pEventData);
	void ModifiedRenderComponentDelegate(IEventDataPtr pEventData);
	void DestroyActorDelegate(IEventDataPtr pEventData);
	void MoveActorDelegate(IEventDataPtr pEventData);

	void SetCamera(std::shared_ptr<CameraNode> camera);
	const std::shared_ptr<CameraNode> GetCamera() const;

	void ActivateScene(bool isActive);

	void PushAndSetMatrix4x4(const DirectX::XMFLOAT4X4& toWorld);
	void PushAndSetMatrix(DirectX::FXMMATRIX toWorld);
	void PopMatrix();
	DirectX::XMMATRIX GetTopMatrix();
	const DirectX::XMFLOAT4X4& GetTopMatrix4x4f();
	DirectX::XMFLOAT4X4 GetTopMatrix4x4fT();
	DirectX::XMMATRIX GetTopInvMatrix();
	DirectX::XMFLOAT4X4 GetTopInvMatrix4x4f();
	DirectX::XMFLOAT4X4 GetTopInvMatrix4x4fT();

	LightManager* GetLightManager();
	ShadowManager* GetShadowManager();

	void AddAlphaSceneNode(AlphaSceneNode* asn);

	IRenderer* GetRenderer();
	RootNode* GetRootNode();
};