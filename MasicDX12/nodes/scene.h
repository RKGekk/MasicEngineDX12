#pragma once

#include <memory>
#include <string>

#include "scene_node.h"

class LightManager;
class QualifierNode;

class Scene {
public:
	Scene();
	virtual ~Scene();

	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate();
	
	bool AddChild(std::shared_ptr<SceneNode> kid);
	bool RemoveChild(std::shared_ptr<SceneNode> kid);

	void ActivateScene(bool is_active);

	std::shared_ptr<LightManager> GetLightManager();
	std::shared_ptr<QualifierNode> GetRootNode();

protected:
	bool m_scene_active = true;

	std::shared_ptr<QualifierNode> m_root_node;
	std::shared_ptr<LightManager> m_light_manager;
};