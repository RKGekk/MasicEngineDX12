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
#include "scene_node_component_interface.h"
#include "../nodes/light_type.h"

class CommandList;
class Material;
class Mesh;
class LightNode;

class LightComponent : public SceneNodeComponentInterface {
public:
	static const std::string g_Name;

	LightComponent();
	LightComponent(const pugi::xml_node& data);

	virtual bool VInit(const pugi::xml_node& data) override;
	virtual void VPostInit() override;
	virtual void VUpdate(const GameTimerDelta& delta) override;

	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	virtual std::shared_ptr<SceneNode> VGetSceneNode() override;

private:
	LightType GetLightType(const std::string& light_type_string);

	std::shared_ptr<LightNode> m_scene_node;

	bool Init(const pugi::xml_node& data);
};