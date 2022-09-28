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
#include "../nodes/light_type.h"

class CommandList;
class Material;
class Mesh;
class LightNode;

class LightComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	LightComponent();
	LightComponent(const pugi::xml_node& data);
	virtual ~LightComponent();

	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	virtual std::shared_ptr<LightNode> VGetLightNode();

protected:
	virtual bool VDelegateInit(const pugi::xml_node& data) override;
	virtual void VDelegatePostInit() override;
	virtual void VDelegateUpdate(const GameTimerDelta& delta) override;

private:
	LightType GetLightType(const std::string& light_type_string);

	std::shared_ptr<LightNode> m_loaded_scene_node;
};