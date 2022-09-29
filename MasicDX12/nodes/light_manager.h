#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>

#include "../graphics/directx12_wrappers/command_list.h"
#include "light_type.h"
#include "../graphics/directional_light.h"
#include "../graphics/point_light.h"
#include "../graphics/spot_light.h"

class LightNode;
class SceneNode;

class LightManager {
public:
	friend class Scene;
	using Lights = std::unordered_set<std::shared_ptr<LightNode>>;
	using LightIndexMap = std::unordered_map<std::shared_ptr<LightNode>, uint32_t>;

	LightManager();
	void CalcLighting(DirectX::FXMMATRIX view);
	void CopyDirectionalLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	void CopyPointLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	void CopySpotLighting(CommandList& command_list, uint32_t slot, SceneNode* pNode);
	int GetLightCount(std::shared_ptr<SceneNode> node);

	void AddLight(std::shared_ptr<SceneNode> node);
	void RemoveLight(std::shared_ptr<SceneNode> node);

	std::vector<DirectionalLight>& GetDirLights();
	size_t GetDirLightsCount();

	std::vector<PointLight>& GetPointLights();
	size_t GetPointLightsCount();

	std::vector<SpotLight>& GetSpotLights();
	size_t GetSpotLightsCount();

protected:
	void ManageInsert(std::shared_ptr<LightNode> light);
	void ManageDelete(std::shared_ptr<LightNode> light_node);

	Lights m_lights;
	LightIndexMap m_index_map;
	std::vector<DirectionalLight> m_dir_lights;
	std::vector<PointLight> m_point_lights;
	std::vector<SpotLight> m_spot_lights;
};