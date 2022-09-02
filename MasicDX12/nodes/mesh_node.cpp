#include "mesh_node.h"

#include <algorithm>

MeshNode::MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform) : SceneNode(name, &transform) {}

MeshNode::MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform, MeshList meshes) : SceneNode(name, &transform), m_meshes(std::move(meshes)) {}

MeshNode::MeshNode(const std::string& name, DirectX::FXMMATRIX transform) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true) {}

MeshNode::MeshNode(const std::string& name, DirectX::FXMMATRIX transform, MeshList meshes) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true), m_meshes(std::move(meshes)) {}

HRESULT MeshNode::VOnRestore() {
	return S_OK;
}

bool MeshNode::AddMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return false;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.cend()) return false;

    m_meshes.push_back(mesh);
    DirectX::BoundingBox::CreateMerged(m_AABB_merged, m_AABB_merged, mesh->GetAABB());
    DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere_merged, m_AABB_merged);

    return true;
}

void MeshNode::RemoveMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.end()) {
        m_meshes.erase(iter);
    }
}

const MeshNode::MeshList& MeshNode::GetMeshes() {
    return m_meshes;
}

std::shared_ptr<Mesh> MeshNode::GetMesh(size_t index) {
    std::shared_ptr<Mesh> mesh;

    if (index < m_meshes.size()) {
        mesh = m_meshes[index];
    }

    return mesh;
}

const DirectX::BoundingBox& MeshNode::GetAABB() const {
	return m_AABB_merged;
}

const DirectX::BoundingSphere& MeshNode::GetSphere() const {
    return m_sphere_merged;
}
