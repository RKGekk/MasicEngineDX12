#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <pugixml/pugixml.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>

#include "actor_component.h"
#include "base_scene_node_component.h"

class CommandList;
class Material;
class Mesh;

class MeshComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	MeshComponent();
	MeshComponent(const pugi::xml_node& data);
	virtual ~MeshComponent();

	virtual const std::string& VGetName() const override;
	virtual pugi::xml_node VGenerateXml() override;

	const std::string& GetResourceName();
	const std::string& GetResourceDirecory();

protected:
	virtual bool VDelegateInit(const pugi::xml_node& data) override;
	virtual void VDelegatePostInit() override;
	virtual void VDelegateUpdate(const GameTimerDelta& delta) override;

private:
	std::string m_resource_name;
	std::string m_resource_directory;

	std::shared_ptr<SceneNode> m_loaded_scene_node;

	bool LoadModel(const std::filesystem::path& file_name, bool is_instanced, bool is_inv_y_texture);
	static std::shared_ptr<SceneNode> DeepCopyNode(const std::shared_ptr<SceneNode>& node, bool is_instanced);
};