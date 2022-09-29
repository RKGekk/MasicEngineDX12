#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../graphics/directx12_wrappers/command_list.h"

class MeshNode;
class SceneNode;

class MeshManager {
	friend class Scene;
public:
	using MeshName = std::string;
	using MeshList = std::unordered_set<std::shared_ptr<MeshNode>>;
	using MeshMap = std::unordered_map<MeshName, MeshList>;
	using SiblingsByParentMap = std::unordered_map<std::shared_ptr<SceneNode>, MeshMap>;

	MeshManager();

	void AddMesh(std::shared_ptr<SceneNode> node);
	void RemoveMesh(std::shared_ptr<SceneNode> node);

	int GetMeshCount(std::shared_ptr<SceneNode> node);
	int GetMeshCount(MeshName mesh_name);
	const MeshList& GetMeshList(MeshName mesh_name) const;
	const MeshMap& GetMeshMap() const;

	int CountSiblingsByParent(const std::shared_ptr<SceneNode>& parent) const;
	const MeshMap& GetSiblingsByParent(const std::shared_ptr<SceneNode>& parent) const;
	int CountSiblingsMeshList(const std::shared_ptr<SceneNode>& parent, MeshName mesh_name) const;
	const MeshList& GetSiblingsMeshList(const std::shared_ptr<SceneNode>& parent, MeshName mesh_name) const;
	const SiblingsByParentMap& GetSiblingsByParentMap() const;

protected:
	void ManageAddSiblingsMap(std::shared_ptr<MeshNode> mesh_node);
	void ManageDeleteSiblingsMap(std::shared_ptr<MeshNode> mesh_node);

	MeshMap m_mesh_map;
	SiblingsByParentMap m_siblings_by_parent_map;
};