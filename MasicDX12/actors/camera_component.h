#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <map>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <pugixml/pugixml.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include "actor_component.h"
#include "scene_node_component_interface.h"

class CommandList;
class Material;
class Mesh;

class CameraComponent : public SceneNodeComponentInterface {
public:
	static const std::string g_Name;

	CameraComponent();
	CameraComponent(const pugi::xml_node& data);

	virtual bool VInit(const pugi::xml_node& data) override;
	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	virtual std::shared_ptr<SceneNode> VGetSceneNode() override;

private:
	std::shared_ptr<SceneNode> m_scene_node;
	float m_fov;
	float m_near;
	float m_far;

	bool Init(const pugi::xml_node& data);
};