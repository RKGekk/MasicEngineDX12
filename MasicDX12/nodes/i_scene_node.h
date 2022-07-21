#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <Windows.h>

#include "scene_node_properties.h"
#include "ray_cast.h"
#include "../actors/actor.h"

class Scene;
class ISceneNode;

struct actor_component_hash {
	std::size_t operator () (const std::pair<ActorId, ComponentId>& p) const {
		return std::hash<ActorId>{}(p.first) ^ p.second;
		//return (p.first << 16U) + p.second;
	}
};

typedef std::unordered_map<std::pair<ActorId, ComponentId>, std::shared_ptr<ISceneNode>, actor_component_hash> SceneActorMap;
typedef std::unordered_map<ActorId, std::unordered_set<ComponentId>> ActorComponentMap;

class ISceneNode {
public:
	virtual const SceneNodeProperties& VGet() const = 0;

	virtual void VSetTransform(DirectX::FXMMATRIX toWorld, DirectX::CXMMATRIX fromWorld, bool calulate_from) = 0;
	virtual void VSetTransform4x4(const DirectX::XMFLOAT4X4* toWorld, const DirectX::XMFLOAT4X4* fromWorld) = 0;

	virtual HRESULT VOnUpdate(Scene* pScene, float elapsedSeconds) = 0;
	virtual HRESULT VOnRestore(Scene* pScene) = 0;

	virtual HRESULT VPreRender(Scene* pScene) = 0;
	virtual bool VIsVisible(Scene* pScene) const = 0;
	virtual HRESULT VRender(Scene* pScene) = 0;
	virtual HRESULT VRenderChildren(Scene* pScene) = 0;
	virtual HRESULT VPostRender(Scene* pScene) = 0;

	virtual HRESULT VShadowPreRender(Scene* pScene) = 0;
	virtual HRESULT VShadowRender(Scene* pScene) = 0;
	virtual HRESULT VShadowRenderChildren(Scene* pScene) = 0;
	virtual HRESULT VShadowPostRender(Scene* pScene) = 0;

	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) = 0;
	virtual bool VRemoveChild(ActorId aid, ComponentId cid) = 0;
	virtual HRESULT VOnLostDevice(Scene* pScene) = 0;
	virtual HRESULT VPick(Scene* pScene, RayCast* pRayCast) = 0;

	virtual ISceneNode* VGetParent() = 0;

	virtual ~ISceneNode() {};
};