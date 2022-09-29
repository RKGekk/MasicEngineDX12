#pragma once

#include <memory>
#include <string>

#include "scene_node.h"

class LightManager;
class MeshManager;
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
	std::shared_ptr<MeshManager> GetMeshManager();
	std::shared_ptr<QualifierNode> GetRootNode();

protected:
	void ManageAddNodes(std::shared_ptr<SceneNode> node);
	void ManageRemoveNodes(std::shared_ptr<SceneNode> node);

	bool m_scene_active = true;

	std::shared_ptr<QualifierNode> m_root_node;
	std::shared_ptr<LightManager> m_light_manager;
	std::shared_ptr<MeshManager> m_mesh_manager;
};