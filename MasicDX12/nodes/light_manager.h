#pragma once

#include <set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>

#include "light_node.h"
#include "light_type.h"
#include "../graphics/directx12_wrappers/command_list.h"

class LightManager {
public:
	friend class Scene;
	using Lights = std::set<std::shared_ptr<LightNode>>;
	using LightIndexMap = std::unordered_map<Lights::iterator, uint32_t>;

	LightManager();
	void CalcLighting(DirectX::FXMMATRIX view);
	void CopyDirectionalLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	void CopyPointLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	void CopySpotLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	int GetLightCount(const SceneNode* node);

	void AddLight(std::shared_ptr<LightNode> light);
	void RemoveLight(std::shared_ptr<LightNode> light);

	std::vector<DirectionalLight>& GetDirLights();
	size_t GetDirLightsCount();

	std::vector<PointLight>& GetPointLights();
	size_t GetPointLightsCount();

	std::vector<SpotLight>& GetSpotLights();
	size_t GetSpotLightsCount();

protected:
	Lights m_lights;
	LightIndexMap m_index_map;
	std::vector<DirectionalLight> m_dir_lights;
	std::vector<PointLight> m_point_lights;
	std::vector<SpotLight> m_spot_lights;
};