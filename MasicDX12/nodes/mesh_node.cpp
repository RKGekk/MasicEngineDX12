#include "mesh_node.h"

#include "../tools/memory_utility.h"

#include <algorithm>

MeshNode::MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform) : SceneNode(name, &transform), m_instanced(false) {}

MeshNode::MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform, const MeshList& meshes) : SceneNode(name, &transform), m_instanced(false) {
    for (const auto& mesh : meshes) {
        AddMesh(mesh);
    }
}

MeshNode::MeshNode(const std::string& name, DirectX::FXMMATRIX transform) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true), m_instanced(false) {}

MeshNode::MeshNode(const std::string& name, DirectX::FXMMATRIX transform, const MeshList& meshes) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true), m_instanced(false) {
    for (const auto& mesh : meshes) {
        AddMesh(mesh);
    }
}

HRESULT MeshNode::VOnRestore() {
	return S_OK;
}

HRESULT MeshNode::VOnUpdate() {
    HRESULT hr = SceneNode::VOnUpdate();
    return hr;
}

bool MeshNode::AddMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return false;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.cend()) return false;
    AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    m_meshes.push_back(mesh);
    CalcAABB();

    return true;
}

void MeshNode::RemoveMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter == m_meshes.end()) return;

    m_meshes.erase(iter);
    AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    CalcAABB();
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
bool MeshNode::GetIsInstanced() const {
    return m_instanced;
}

void MeshNode::SetIsInstanced(bool is_instanced) {
    m_instanced = is_instanced;
}

void MeshNode::CalcAABB() {
    DirectX::BoundingBox aabb_max;
    int sz = m_meshes.size();
    for (int i = 0u; i < sz; ++i) {
        const auto& mesh = m_meshes[i];
        DirectX::BoundingBox aabb_original = mesh->GetAABB();
        if (i == 0) aabb_max = aabb_original;
        else DirectX::BoundingBox::CreateMerged(aabb_max, aabb_max, aabb_original);
        SetAABB(aabb_max);
    }
    AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
}
