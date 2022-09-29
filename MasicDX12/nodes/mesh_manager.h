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

	MeshManager();

	void AddMesh(std::shared_ptr<SceneNode> node);
	void RemoveMesh(std::shared_ptr<SceneNode> node);

	int GetMeshCount(std::shared_ptr<SceneNode> node);
	int GetMeshCount(MeshName mesh_name);
	const MeshList& GetMeshList(MeshName mesh_name) const;
	const MeshMap& GetMeshMap() const;

protected:
	MeshMap m_mesh_map;
};