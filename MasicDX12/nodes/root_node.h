#pragma once

#include <memory>

#include <Windows.h>

#include "../actors/actor.h"
#include "scene_node.h"

class RootNode : public SceneNode {
public:
	RootNode();
	virtual bool VAddChild(std::shared_ptr<ISceneNode> kid) override;
	virtual HRESULT VRenderChildren(Scene* pScene) override;
	virtual bool VRemoveChild(ActorId aid, ComponentId cid) override;
	virtual bool VIsVisible(Scene* pScene) const override;

	ISceneNode* GetRenderPass(RenderPass pass);
};