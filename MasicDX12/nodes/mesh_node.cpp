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
    if (Get().GetDirtyFlags() & to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh)) {
        RecalcAABB();
        DirectX::BoundingBox aabb_max = m_AABB_merged;
        std::shared_ptr<SceneNode> parent = m_pParent.lock();
        while (parent) {
            std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(parent);
            if (mesh_node) {
                mesh_node->RecalcAABB();
                DirectX::BoundingBox::CreateMerged(aabb_max, mesh_node->m_AABB_merged, aabb_max);
                mesh_node->m_AABB_merged = aabb_max;
                DirectX::BoundingSphere::CreateFromBoundingBox(mesh_node->m_sphere_merged, mesh_node->m_AABB_merged);
            }
            parent = parent->GetParent();
        }
    }

    return SceneNode::VOnUpdate();
}

bool MeshNode::AddMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return false;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.cend()) return false;
    SetDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    m_meshes.push_back(mesh);
    DirectX::BoundingBox aabb_original = mesh->GetAABB();
    DirectX::BoundingBox aabb_transformed;
    aabb_original.Transform(aabb_transformed, m_props.FullCumulativeToWorld());
    if (m_meshes.size() == 1u) {
        m_AABB_merged = aabb_transformed;
        DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere_merged, m_AABB_merged);
    }
    else {
        DirectX::BoundingBox::CreateMerged(m_AABB_merged, m_AABB_merged, aabb_transformed);
        DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere_merged, m_AABB_merged);
    }

    return true;
}

void MeshNode::RemoveMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.end()) {
        m_meshes.erase(iter);
    }
    RecalcAABB();
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

bool MeshNode::GetIsInstanced() const {
    return m_instanced;
}

void MeshNode::SetIsInstanced(bool is_instanced) {
    m_instanced = is_instanced;
}

void MeshNode::RecalcAABB() {
    DirectX::BoundingBox aabb_max;
    int sz = m_meshes.size();
    for (int i = 0u; i < sz; ++i) {
        const auto& mesh = m_meshes[i];
        DirectX::BoundingBox aabb_original = mesh->GetAABB();
        DirectX::BoundingBox aabb_transformed;
        aabb_original.Transform(aabb_transformed, m_props.FullCumulativeToWorld());
        if (i == 0) aabb_max = aabb_transformed;
        else DirectX::BoundingBox::CreateMerged(aabb_max, aabb_max, aabb_transformed);
        m_AABB_merged = aabb_max;
        DirectX::BoundingSphere::CreateFromBoundingBox(m_sphere_merged, m_AABB_merged);
    }
    SetDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
}
