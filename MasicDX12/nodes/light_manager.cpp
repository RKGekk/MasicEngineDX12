#include "light_manager.h"

#include <DirectXMath.h>
#include <cassert>

#include "light_node.h"

LightManager::LightManager() {}

void LightManager::CalcLighting(DirectX::FXMMATRIX view) {
	for (auto it = m_lights.begin(); it != m_lights.end(); ++it) {
		const auto& light_node = *it;
		//if (!(light_node->Get().GetDirtyFlags() & to_underlying(SceneNodeProperties::DirtyFlags::DF_Light))) continue;
		LightType lt = light_node->VGetLight().m_light_type;
		switch (lt) {
			case LightType::DIRECTIONAL: m_dir_lights[m_index_map[light_node]] = light_node->GetDirectionalLight(view); break;
			case LightType::POINT: m_point_lights[m_index_map[light_node]] = light_node->GetPointLight(view); break;
			case LightType::SPOT: m_spot_lights[m_index_map[light_node]] = light_node->GetSpotLight(view); break;
			default: assert(false); break;
		}
	}
}

void LightManager::CopyDirectionalLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode) {
	command_list.SetGraphicsDynamicStructuredBuffer(slot, m_dir_lights);
}

void LightManager::CopyPointLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode) {
	command_list.SetGraphicsDynamicStructuredBuffer(slot, m_point_lights);
}


void LightManager::CopySpotLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode) {
	command_list.SetGraphicsDynamicStructuredBuffer(slot, m_spot_lights);
}

int LightManager::GetLightCount(std::shared_ptr<SceneNode> node) {
	return m_lights.size();
}


void LightManager::AddLight(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<LightNode> light_node = std::dynamic_pointer_cast<LightNode>(node);
	if (light_node) {
		ManageInsert(light_node);
	}
	const SceneNodeList& children = node->VGetChildren();
	auto i = children.cbegin();
	auto end = children.cend();
	while (i != end) {
		AddLight(*i);
		++i;
	}
}

void LightManager::RemoveLight(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<LightNode> light_node = std::dynamic_pointer_cast<LightNode>(node);
	if (light_node) {
		ManageDelete(light_node);
	}
	const SceneNodeList& children = node->VGetChildren();
	auto i = children.cbegin();
	auto end = children.cend();
	while (i != end) {
		RemoveLight(*i);
		++i;
	}
}

std::vector<DirectionalLight>& LightManager::GetDirLights() {
	return m_dir_lights;
}

size_t LightManager::GetDirLightsCount() {
	return m_dir_lights.size();
}

std::vector<PointLight>& LightManager::GetPointLights() {
	return m_point_lights;
}

size_t LightManager::GetPointLightsCount() {
	return m_point_lights.size();
}

std::vector<SpotLight>& LightManager::GetSpotLights() {
	return m_spot_lights;
}

size_t LightManager::GetSpotLightsCount() {
	return m_spot_lights.size();
}

void LightManager::ManageInsert(std::shared_ptr<LightNode> light) {
	auto res = m_lights.insert(light);
	if (!res.second) return;

	LightType lt = light->VGetLight().m_light_type;
	switch (lt) {
		case LightType::DIRECTIONAL:
		{
			m_index_map.insert({ *res.first, m_dir_lights.size() });
			m_dir_lights.push_back(light->GetDirectionalLight(DirectX::XMMatrixIdentity()));
		}
		break;
		case LightType::POINT:
		{
			m_index_map.insert({ *res.first, m_point_lights.size() });
			m_point_lights.push_back(light->GetPointLight(DirectX::XMMatrixIdentity()));
		}
		break;
		case LightType::SPOT:
		{
			m_index_map.insert({ *res.first, m_spot_lights.size() });
			m_spot_lights.push_back(light->GetSpotLight(DirectX::XMMatrixIdentity()));
		}
		break;
		default:
			assert(false);
			break;
	}
}

void LightManager::ManageDelete(std::shared_ptr<LightNode> light_node) {
	Lights::iterator it = m_lights.find(light_node);
	if (it == m_lights.end()) return;

	LightType lt = light_node->VGetLight().m_light_type;
	switch (lt) {
		case LightType::DIRECTIONAL:
		{
			uint32_t index = m_index_map[light_node];
			m_dir_lights.erase(m_dir_lights.begin() + index);
			m_index_map.erase(light_node);
			for (auto& p : m_index_map) {
				if ((p.first)->VGetLight().m_light_type == LightType::DIRECTIONAL && p.second > index) --p.second;
			}
		}
		break;
		case LightType::POINT:
		{
			uint32_t index = m_index_map[light_node];
			m_point_lights.erase(m_point_lights.begin() + index);
			m_index_map.erase(light_node);
			for (auto& p : m_index_map) {
				if ((p.first)->VGetLight().m_light_type == LightType::POINT && p.second > index) --p.second;
			}
		}
		break;
		case LightType::SPOT:
		{
			uint32_t index = m_index_map[light_node];
			m_spot_lights.erase(m_spot_lights.begin() + index);
			m_index_map.erase(light_node);
			for (auto& p : m_index_map) {
				if ((p.first)->VGetLight().m_light_type == LightType::SPOT && p.second > index) --p.second;
			}
		}
		break;
		default:
			assert(false);
			break;
	}

	size_t ct = m_lights.erase(light_node);
}
