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
#include "base_scene_node_component.h"

class CommandList;
class Material;
class Mesh;

class MeshComponent : public BaseSceneNodeComponent {
public:
	static const std::string g_Name;

	MeshComponent();
	MeshComponent(const pugi::xml_node& data);

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

	bool LoadModel(const std::filesystem::path& file_name);

	void ImportScene(CommandList& command_list, const aiScene& scene, std::filesystem::path parent_path);
	void ImportMaterial(CommandList& command_list, const aiMaterial& material, std::filesystem::path parent_path);
	void ImportMesh(CommandList& command_list, const aiMesh& ai_mesh);
	static DirectX::BoundingBox CreateBoundingBox(const aiAABB& aabb);

	std::shared_ptr<SceneNode> ImportSceneNode(CommandList& command_list, std::shared_ptr<SceneNode> parent, const aiNode* aiNode);

	using MaterialMap = std::map<std::string, std::shared_ptr<Material>>;
	using MaterialList = std::vector<std::shared_ptr<Material>>;
	using MeshList = std::vector<std::shared_ptr<Mesh>>;

	MaterialMap m_material_map;
	MaterialList m_materials;
	MeshList m_meshes;
};