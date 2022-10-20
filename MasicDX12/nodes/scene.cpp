#include "scene.h"

#include <functional>

#include "qualifier_node.h"
#include "camera_node.h"
#include "light_node.h"
#include "mesh_node.h"
#include "shadow_camera_node.h"
#include "../events/evt_data_new_render_component.h"
#include "../events/evt_data_destroy_actor.h"
#include "../events/evt_data_move_actor.h"
#include "../events/evt_data_modified_render_component.h"
#include "light_manager.h"
#include "mesh_manager.h"
#include "shadow_manager.h"
#include "light_node.h"
#include "../engine/engine.h"

void Scene::ActivateScene(bool is_active) {
	m_scene_active = is_active;
}

std::shared_ptr<LightManager> Scene::GetLightManager() {
	return m_light_manager;
}

std::shared_ptr<ShadowManager> Scene::GetShadowManager() {
	return m_shadow_manager;
}

std::shared_ptr<MeshManager> Scene::GetMeshManager() {
	return m_mesh_manager;
}

std::shared_ptr<QualifierNode> Scene::GetRootNode() {
	return m_root_node;
}

const Scene::SceneConfig& Scene::GetSceneConfig() {
	return m_scene_config;
}

void Scene::SetSceneConfig(const SceneConfig& scene_config) {
	m_scene_config = scene_config;
}

void Scene::ManageAddNodes(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	if (std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(node)) {
		m_light_manager->AddLight(node);
	};
	if (std::shared_ptr<ShadowCameraNode> pShadow = std::dynamic_pointer_cast<ShadowCameraNode>(node)) {
		m_shadow_manager->AddShadow(node);
	};
	if (std::shared_ptr<MeshNode> pMesh = std::dynamic_pointer_cast<MeshNode>(node)) {
		if (pMesh->GetIsInstanced()) {
			m_mesh_manager->AddMesh(node);
		}
	};
	for (auto& current_node : node->m_children) {
		ManageAddNodes(current_node);
	}
}

void Scene::ManageRemoveNodes(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	if (std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(node)) {
		m_light_manager->RemoveLight(node);
	};
	if (std::shared_ptr<MeshNode> pMesh = std::dynamic_pointer_cast<MeshNode>(node)) {
		m_mesh_manager->RemoveMesh(node);
	};
	for (auto& current_node : node->m_children) {
		ManageRemoveNodes(current_node);
	}
}

Scene::Scene() {
	using namespace std::literals;
	m_root_node = std::make_shared<QualifierNode>("Root Node"s);
	m_light_manager = std::make_shared<LightManager>();
	m_shadow_manager = std::make_shared<ShadowManager>();
	m_mesh_manager = std::make_shared<MeshManager>();

	m_scene_config.FogColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_scene_config.FogStart = 1.0f;
	m_scene_config.FogRange = 100.0f;
}

Scene::~Scene() {}

HRESULT Scene::OnRestore() {
	if (!m_root_node) { return S_OK; }
	return m_root_node->VOnRestore();
}

HRESULT Scene::OnLostDevice() {
	if (m_root_node) {
		return m_root_node->VOnLostDevice();
	}
	return S_OK;
}

HRESULT Scene::OnUpdate() {
	if (!m_root_node) {
		return S_OK;
	}
	return m_root_node->VOnUpdate();
}

bool Scene::AddChild(std::shared_ptr<SceneNode> kid) {
	if (!kid) return false;
	ManageAddNodes(kid);
	return m_root_node->VAddChild(kid);
}

bool Scene::RemoveChild(std::shared_ptr<SceneNode> kid) {
	ManageRemoveNodes(kid);
	return true;
}