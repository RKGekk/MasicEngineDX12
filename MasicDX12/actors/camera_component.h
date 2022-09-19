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
class CameraNode;

class CameraComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	CameraComponent();
	CameraComponent(const pugi::xml_node& data);

	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	virtual std::shared_ptr<CameraNode> VGetCameraNode();

protected:
	virtual bool VDelegateInit(const pugi::xml_node& data) override;
	virtual void VDelegatePostInit() override;
	virtual void VDelegateUpdate(const GameTimerDelta& delta) override;

private:
	std::shared_ptr<CameraNode> m_loaded_scene_node;
	float m_fov;
	float m_near;
	float m_far;
	float m_aspect_ratio;

	bool Init(const pugi::xml_node& data);
};