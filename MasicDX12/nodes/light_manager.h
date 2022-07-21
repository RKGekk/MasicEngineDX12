#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>

#include "light_node.h"
#include "../graphics/constant_buffer_types.h"

#define MAXIMUM_LIGHTS_SUPPORTED (25)

class LightManager {
	friend class Scene;

protected:
	Lights m_Lights;
	std::vector<DirectionalLight> gDirLights;
	std::vector<PointLight> gPointLights;
	std::vector<SpotLight> gSpotLights;

public:
	LightManager();
	void CalcLighting(Scene* pScene);
	void CopyLighting(CB_PS_PixelShader_Light* pLighting, SceneNode* pNode);
	void CopyLighting(CB_PS_PixelShader_Light_All* pLighting, SceneNode* pNode);
	int GetLightCount(const SceneNode* node);

	std::vector<DirectionalLight>& GetDirLights();
	std::vector<PointLight>& GetPointLights();
	std::vector<SpotLight>& GetSpotLights();
};