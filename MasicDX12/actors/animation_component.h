#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

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
class SkinnedData;

class AnimationComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	AnimationComponent();
	AnimationComponent(const pugi::xml_node& data);
	virtual ~AnimationComponent();

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
	std::shared_ptr<SkinnedData> m_skinned_data;

	std::shared_ptr<SceneNode> ImportScene2(CommandList& command_list, const aiScene& scene, std::filesystem::path parent_path, const std::string& file_name, bool is_inv_y_texture);
	std::shared_ptr<SceneNode> ImportSceneNode(CommandList& command_list, const std::filesystem::path& file_name, bool is_inv_y_texture);
	bool LoadModel(const std::filesystem::path& file_name, bool is_instanced, bool is_inv_y_texture);
	std::shared_ptr<SceneNode> DeepCopyNode(const std::shared_ptr<SceneNode>& node, bool is_instanced);
};