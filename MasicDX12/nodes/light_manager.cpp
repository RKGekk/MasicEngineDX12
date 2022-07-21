#include "light_manager.h"
#include "scene.h"

LightManager::LightManager() {
	gDirLights.reserve(MAXIMUM_LIGHTS_SUPPORTED);
	gPointLights.reserve(MAXIMUM_LIGHTS_SUPPORTED);
	gSpotLights.reserve(MAXIMUM_LIGHTS_SUPPORTED);
}

void LightManager::CalcLighting(Scene* pScene) {
	//pScene->GetRenderer()->VCalcLighting(&m_Lights, MAXIMUM_LIGHTS_SUPPORTED);
	gDirLights.clear();
	gPointLights.clear();
	gSpotLights.clear();
	for (Lights::iterator i = m_Lights.begin(); i != m_Lights.end(); ++i) {
		std::shared_ptr<LightNode> light = *i;

		const LightProperties& props = light->VGetLight();
		switch (props.m_LightType) {
		case LightType::DIRECTIONAL:
		{
			DirectionalLight dl;
			dl.Ambient = props.m_Ambient;
			dl.Diffuse = props.m_Diffuse;
			dl.Specular = props.m_Specular;
			dl.Direction = light->GetDirection3f();
			gDirLights.push_back(dl);
		}
		break;
		case LightType::POINT:
		{
			PointLight pl;
			pl.Ambient = props.m_Ambient;
			pl.Diffuse = props.m_Diffuse;
			pl.Specular = props.m_Specular;
			pl.Position = light->GetPosition3();
			pl.Att.x = props.m_Attenuation[0];
			pl.Att.y = props.m_Attenuation[1];
			pl.Att.z = props.m_Attenuation[2];
			pl.Range = props.m_Range;
			gPointLights.push_back(pl);
		}
		break;
		case LightType::SPOT:
		{
			SpotLight sl;
			sl.Ambient = props.m_Ambient;
			sl.Diffuse = props.m_Diffuse;
			sl.Specular = props.m_Specular;
			sl.Position = light->GetPosition3();
			sl.Att.x = props.m_Attenuation[0];
			sl.Att.y = props.m_Attenuation[1];
			sl.Att.z = props.m_Attenuation[2];
			sl.Range = props.m_Range;
			sl.Direction = light->GetDirection3f();
			sl.Spot = props.m_Spot;
			gSpotLights.push_back(sl);
		}
		break;
		}
	}
}

void LightManager::CopyLighting(CB_PS_PixelShader_Light* pLighting, SceneNode* pNode) {
	int count = GetLightCount(pNode);
	if (count) {
		for (size_t i = 0; i < gDirLights.size(); ++i) {
			pLighting->gDirLights[i] = gDirLights[i];
		}
	}
}

void LightManager::CopyLighting(CB_PS_PixelShader_Light_All* pLighting, SceneNode* pNode) {
	int count = GetLightCount(pNode);
	if (count) {
		pLighting->gDirLightCount = gDirLights.size();
		for (size_t i = 0; i < gDirLights.size(); ++i) {
			pLighting->gDirLights[i] = gDirLights[i];
		}
		pLighting->gPointLightCount = gPointLights.size();
		for (size_t i = 0; i < gPointLights.size(); ++i) {
			pLighting->gPointLights[i] = gPointLights[i];
		}
		pLighting->gSpotLightCount = gSpotLights.size();
		for (size_t i = 0; i < gSpotLights.size(); ++i) {
			pLighting->gSpotLights[i] = gSpotLights[i];
		}
	}
}

int LightManager::GetLightCount(const SceneNode* node) {
	return m_Lights.size();
}

std::vector<DirectionalLight>& LightManager::GetDirLights() {
	return gDirLights;
}

std::vector<PointLight>& LightManager::GetPointLights() {
	return gPointLights;
}

std::vector<SpotLight>& LightManager::GetSpotLights() {
	return gSpotLights;
}