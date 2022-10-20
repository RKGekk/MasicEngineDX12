#pragma once

#include <memory>
#include <string>

#include "scene_node.h"

class LightManager;
class MeshManager;
class ShadowManager;
class QualifierNode;

class Scene {
public:
	struct SceneConfig {
		DirectX::XMFLOAT4 FogColor;
		float FogStart;
		float FogRange;
	};

	Scene();
	virtual ~Scene();

	HRESULT OnRestore();
	HRESULT OnLostDevice();
	HRESULT OnUpdate();
	
	bool AddChild(std::shared_ptr<SceneNode> kid);
	bool RemoveChild(std::shared_ptr<SceneNode> kid);

	void ActivateScene(bool is_active);

	std::shared_ptr<LightManager> GetLightManager();
	std::shared_ptr<ShadowManager> GetShadowManager();
	std::shared_ptr<MeshManager> GetMeshManager();
	std::shared_ptr<QualifierNode> GetRootNode();

	const SceneConfig& GetSceneConfig();
	void SetSceneConfig(const SceneConfig& scene_config);

protected:
	void ManageAddNodes(std::shared_ptr<SceneNode> node);
	void ManageRemoveNodes(std::shared_ptr<SceneNode> node);

	bool m_scene_active = true;

	std::shared_ptr<QualifierNode> m_root_node;
	std::shared_ptr<LightManager> m_light_manager;
	std::shared_ptr<ShadowManager> m_shadow_manager;
	std::shared_ptr<MeshManager> m_mesh_manager;

	SceneConfig m_scene_config;
};