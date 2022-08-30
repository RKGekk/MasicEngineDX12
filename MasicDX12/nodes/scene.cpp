#include "scene.h"
#include "qualifier_node.h"
#include "camera_node.h"
#include "../events/evt_data_new_render_component.h"
#include "../events/evt_data_destroy_actor.h"
#include "../events/evt_data_move_actor.h"
#include "../events/evt_data_modified_render_component.h"
#include "light_manager.h"
#include "shadow_manager.h"
#include "../engine/engine.h"

void Scene::ActivateScene(bool is_active) {
	m_scene_active = is_active;
}

std::shared_ptr<LightManager> Scene::GetLightManager() {
	return m_light_manager;
}

std::shared_ptr<QualifierNode> Scene::GetRootNode() {
	return m_root_node;
}

Scene::Scene() {
	using namespace std::literals;
	m_root_node = std::make_shared<QualifierNode>("Root Node"s);
	m_light_manager = std::make_shared<LightManager>();
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
	std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(kid);
	if (pLight) {
		m_light_manager->AddLight(pLight);
	}
	return m_root_node->VAddChild(kid);
}

bool Scene::RemoveChild(std::shared_ptr<SceneNode> kid) {
	std::shared_ptr<LightNode> pLight = std::dynamic_pointer_cast<LightNode>(kid);
	if (pLight) {
		m_light_manager->RemoveLight(pLight);
	}
	return m_root_node->VRemoveChild(kid);
}