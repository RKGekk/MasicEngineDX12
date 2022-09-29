#include "mesh_manager.h"

#include "scene_node.h"
#include "mesh_node.h"

MeshManager::MeshManager() {}

void MeshManager::AddMesh(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(node);
	if (mesh_node) {
		const std::string& name = mesh_node->Get().Name();
		m_mesh_map[name].insert(mesh_node);
	}
	const SceneNodeList& children = node->VGetChildren();
	auto i = children.cbegin();
	auto end = children.cend();
	while (i != end) {
		AddMesh(*i);
		++i;
	}
}

void MeshManager::RemoveMesh(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(node);
	if (mesh_node) {
		const std::string& name = mesh_node->Get().Name();
		m_mesh_map[name].erase(mesh_node);
	}
	const SceneNodeList& children = node->VGetChildren();
	auto i = children.cbegin();
	auto end = children.cend();
	while (i != end) {
		RemoveMesh(*i);
		++i;
	}
}

int MeshManager::GetMeshCount(std::shared_ptr<SceneNode> node) {
	return m_mesh_map.size();
}

int MeshManager::GetMeshCount(MeshName mesh_name) {
	return m_mesh_map.count(mesh_name);
}

const MeshManager::MeshList& MeshManager::GetMeshList(MeshName mesh_name) const {
	return m_mesh_map.at(mesh_name);
}

const MeshManager::MeshMap& MeshManager::GetMeshMap() const {
	return m_mesh_map;
}
