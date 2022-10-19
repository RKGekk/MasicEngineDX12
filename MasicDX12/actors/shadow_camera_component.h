#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <map>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <pugixml/pugixml.hpp>

#include "actor_component.h"
#include "base_scene_node_component.h"

class CommandList;
class Material;
class Mesh;
class ShadowCameraNode;

class ShadowCameraComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	ShadowCameraComponent();
	ShadowCameraComponent(const pugi::xml_node& data);
	virtual ~ShadowCameraComponent();

	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	virtual std::shared_ptr<ShadowCameraNode> VGetCameraNode();

	int GetSMapWidth();
	int GetSMapHeight();
	int GetDepthBias();
	float GetDepthBiasClamp();
	float GetSlopeScaledDepthBias();

protected:
	virtual bool VDelegateInit(const pugi::xml_node& data) override;
	virtual void VDelegatePostInit() override;
	virtual void VDelegateUpdate(const GameTimerDelta& delta) override;

private:
	std::shared_ptr<ShadowCameraNode> m_loaded_scene_node;

	int m_shadow_map_width;
	int m_shadow_map_height;
	int m_depth_bias;
	float m_depth_bias_clamp;
	float m_slope_scaled_depth_bias;
};