#include "aminated_mesh_node.h"

#include "../tools/memory_utility.h"

#include <algorithm>

AnimatedMeshNode::AnimatedMeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform) : SceneNode(name, &transform), m_instanced(false) {}

AnimatedMeshNode::AnimatedMeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform, const MeshList& meshes) : SceneNode(name, &transform), m_instanced(false) {
    for (const auto& mesh : meshes) {
        AddMesh(mesh);
    }
}

AnimatedMeshNode::AnimatedMeshNode(const std::string& name, DirectX::FXMMATRIX transform) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true), m_instanced(false) {}

AnimatedMeshNode::AnimatedMeshNode(const std::string& name, DirectX::FXMMATRIX transform, const MeshList& meshes) : SceneNode(name, transform, DirectX::XMMatrixIdentity(), true), m_instanced(false) {
    for (const auto& mesh : meshes) {
        AddMesh(mesh);
    }
}

HRESULT AnimatedMeshNode::VOnRestore() {
    return S_OK;
}

HRESULT AnimatedMeshNode::VOnUpdate() {
    HRESULT hr = SceneNode::VOnUpdate();
    return hr;
}

bool AnimatedMeshNode::AddMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return false;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter != m_meshes.cend()) return false;
    AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    m_meshes.push_back(mesh);
    CalcAABB();

    return true;
}

void AnimatedMeshNode::RemoveMesh(std::shared_ptr<Mesh> mesh) {
    if (!mesh) return;

    MeshList::const_iterator iter = std::find(m_meshes.begin(), m_meshes.end(), mesh);
    if (iter == m_meshes.end()) return;

    m_meshes.erase(iter);
    AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    CalcAABB();
}

const AnimatedMeshNode::MeshList& AnimatedMeshNode::GetMeshes() {
    return m_meshes;
}

std::shared_ptr<Mesh> AnimatedMeshNode::GetMesh(size_t index) {
    std::shared_ptr<Mesh> mesh;

    if (index < m_meshes.size()) {
        mesh = m_meshes[index];
    }

    return mesh;
}
bool AnimatedMeshNode::GetIsInstanced() const {
    return m_instanced;
}

void AnimatedMeshNode::SetIsInstanced(bool is_instanced) {
    m_instanced = is_instanced;
}

AnimatedMeshNode::FinalTransformList& AnimatedMeshNode::GetFinalTransformList() {
    return *m_final_transforms;
}

const DirectX::XMFLOAT4X4& AnimatedMeshNode::GetFinalTransform(size_t index) {
    return (*m_final_transforms)[index];
}

void AnimatedMeshNode::SetFinalTransformList(std::shared_ptr<FinalTransformList> final_transform_list) {
    m_final_transforms = std::move(final_transform_list);
}

void AnimatedMeshNode::CalcAABB() {
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
