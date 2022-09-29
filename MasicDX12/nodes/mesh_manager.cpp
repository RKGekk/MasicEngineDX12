#include "mesh_manager.h"

#include "scene_node.h"
#include "mesh_node.h"
#include "qualifier_node.h"

MeshManager::MeshManager() {}

void MeshManager::AddMesh(std::shared_ptr<SceneNode> node) {
	if (!node) return;
	std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(node);
	if (mesh_node) {
		const std::string& name = mesh_node->Get().Name();
		m_mesh_map[name].insert(mesh_node);
		ManageAddSiblingsMap(mesh_node);
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
		ManageDeleteSiblingsMap(mesh_node);
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

int MeshManager::CountSiblingsByParent(const std::shared_ptr<SceneNode>& parent) const {
	return m_siblings_by_parent_map.count(parent);
}

const MeshManager::MeshMap& MeshManager::GetSiblingsByParent(const std::shared_ptr<SceneNode>& parent) const {
	return m_siblings_by_parent_map.at(parent);
}

int MeshManager::CountSiblingsMeshList(const std::shared_ptr<SceneNode>& parent, MeshName mesh_name) const {
	return m_siblings_by_parent_map.at(parent).count(mesh_name);
}

const MeshManager::MeshList& MeshManager::GetSiblingsMeshList(const std::shared_ptr<SceneNode>& parent, MeshName mesh_name) const {
	return m_siblings_by_parent_map.at(parent).at(mesh_name);
}

const MeshManager::SiblingsByParentMap& MeshManager::GetSiblingsByParentMap() const {
	return m_siblings_by_parent_map;
}

void MeshManager::ManageAddSiblingsMap(std::shared_ptr<MeshNode> mesh_node) {
	if (!mesh_node) return;

	const std::string& node_name = mesh_node->Get().Name();
	std::shared_ptr<SceneNode> parent = mesh_node->GetParent();
	if (!parent) return;

	bool have_current_mesh_instaces = m_mesh_map.count(node_name);
	if (!have_current_mesh_instaces) {
		m_siblings_by_parent_map[parent][node_name].insert(mesh_node);
		return;
	}

	for (const auto& same_mesh : m_mesh_map.at(node_name)) {
		std::shared_ptr<SceneNode> current_mesh_parent = same_mesh->GetParent();
		if (current_mesh_parent == parent) {
			m_siblings_by_parent_map[current_mesh_parent][node_name].insert(mesh_node);
			return;
		}
	}
}

void MeshManager::ManageDeleteSiblingsMap(std::shared_ptr<MeshNode> mesh_node) {
	if (m_siblings_by_parent_map.count(mesh_node)) {
		m_siblings_by_parent_map.erase(mesh_node);
	}
}