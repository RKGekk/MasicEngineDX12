#include "mesh_component.h"

#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/device.h"
#include "../graphics/directx12_wrappers/texture.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"
#include "../engine/engine.h"
#include "../graphics/d3d12_renderer.h"
#include "../nodes/mesh_node.h"
#include "transform_component.h"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/anim.h>
#include <assimp/postprocess.h>

#include <cassert>

const std::string MeshComponent::g_Name = "MeshComponent";

MeshComponent::MeshComponent() {}

MeshComponent::MeshComponent(const pugi::xml_node& data) {
	Init(data);
}

bool MeshComponent::VInit(const pugi::xml_node& data) {
	return Init(data);
}

void MeshComponent::VPostInit() {
    std::shared_ptr<Actor> act = GetOwner();
    std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
    if (tc) {
        m_scene_node = std::make_shared<SceneNode>(act->GetName(), tc->GetTransform());
    }
    else {
        m_scene_node = std::make_shared<SceneNode>(act->GetName(), DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity(), false);
    }
    m_scene_node->VAddChild(m_loaded_scene);
}

void MeshComponent::VUpdate(const GameTimerDelta& delta) {
    std::shared_ptr<Actor> act = GetOwner();
    std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
    if (tc) {
        m_scene_node->SetTransform(tc->GetTransform());
    }
}

const std::string& MeshComponent::VGetName() const {
	return MeshComponent::g_Name;
}

pugi::xml_node MeshComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<SceneNode> MeshComponent::VGetSceneNode() {
	return m_scene_node;
}

const std::string& MeshComponent::GetResourceName() {
    return m_resource_name;
}

const std::string& MeshComponent::GetResourceDirecory() {
    return m_resource_directory;
}

bool MeshComponent::Init(const pugi::xml_node& data) {
	m_resource_name = data.child("Mesh").child_value();
	if (m_resource_name.empty()) return false;
	std::filesystem::path p(m_resource_name);
	m_resource_directory = p.parent_path().string();
	return LoadModel(p);
}

bool MeshComponent::LoadModel(const std::filesystem::path& file_name) {
    std::filesystem::path file_path = file_name;
    std::filesystem::path export_path = std::filesystem::path(file_path).replace_extension("assbin");

    std::filesystem::path parent_path;
    if (file_path.has_parent_path()) {
        parent_path = file_path.parent_path();
    }
    else {
        parent_path = std::filesystem::current_path();
    }

    Assimp::Importer importer;
    const aiScene* scene;

    if (std::filesystem::exists(export_path) && std::filesystem::is_regular_file(export_path)) {
        scene = importer.ReadFile(export_path.string(), aiProcess_GenBoundingBoxes);
    }
    else {
        importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

        unsigned int preprocess_flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_OptimizeGraph | aiProcess_ConvertToLeftHanded | aiProcess_GenBoundingBoxes;
        scene = importer.ReadFile(file_path.string(), preprocess_flags);

        if (scene) {
            Assimp::Exporter exporter;
            exporter.Export(scene, "assbin", export_path.string(), 0);
        }
    }

    if (!scene) {
        return false;
    }

    std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(Engine::GetEngine()->GetRenderer());
    std::shared_ptr<Device> device = renderer->GetDevice();
    CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

    ImportScene(*command_list, *scene, parent_path);

    command_queue.ExecuteCommandList(command_list);
    command_queue.Flush();

    return true;
}

void MeshComponent::ImportScene(CommandList& command_list, const aiScene& scene, std::filesystem::path parent_path) {
    if (m_scene_node) {
        m_scene_node.reset();
    }

    m_material_map.clear();
    m_materials.clear();
    m_meshes.clear();

    for (unsigned int i = 0u; i < scene.mNumMaterials; ++i) {
        ImportMaterial(command_list, *(scene.mMaterials[i]), parent_path);
    }

    for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
        ImportMesh(command_list, *(scene.mMeshes[i]));
    }

    m_loaded_scene = ImportSceneNode(command_list, nullptr, scene.mRootNode);
}

void MeshComponent::ImportMaterial(CommandList& command_list, const aiMaterial& material, std::filesystem::path parent_path) {
    aiString material_name;
    aiString aiTexture_path;
    aiTextureOp aiBlend_operation;
    float blend_factor;
    aiColor4D diffuse_color;
    aiColor4D specular_color;
    aiColor4D ambient_color;
    aiColor4D emissive_color;
    float opacity;
    float index_of_refraction;
    float reflectivity;
    float shininess;
    float bump_intensity;

    std::shared_ptr<Material> pMaterial = std::make_shared<Material>();

    if (material.Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == aiReturn_SUCCESS) {
        pMaterial->SetAmbientColor(DirectX::XMFLOAT4(ambient_color.r, ambient_color.g, ambient_color.b, ambient_color.a));
    }
    if (material.Get(AI_MATKEY_COLOR_EMISSIVE, emissive_color) == aiReturn_SUCCESS) {
        pMaterial->SetEmissiveColor(DirectX::XMFLOAT4(emissive_color.r, emissive_color.g, emissive_color.b, emissive_color.a));
    }
    if (material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == aiReturn_SUCCESS) {
        pMaterial->SetDiffuseColor(DirectX::XMFLOAT4(diffuse_color.r, diffuse_color.g, diffuse_color.b, diffuse_color.a));
    }
    if (material.Get(AI_MATKEY_COLOR_SPECULAR, specular_color) == aiReturn_SUCCESS) {
        pMaterial->SetSpecularColor(DirectX::XMFLOAT4(specular_color.r, specular_color.g, specular_color.b, specular_color.a));
    }
    if (material.Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        pMaterial->SetSpecularPower(shininess);
    }
    if (material.Get(AI_MATKEY_OPACITY, opacity) == aiReturn_SUCCESS) {
        pMaterial->SetOpacity(opacity);
    }
    if (material.Get(AI_MATKEY_REFRACTI, index_of_refraction)) {
        pMaterial->SetIndexOfRefraction(index_of_refraction);
    }
    if (material.Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS) {
        pMaterial->SetReflectance(DirectX::XMFLOAT4(reflectivity, reflectivity, reflectivity, reflectivity));
    }
    if (material.Get(AI_MATKEY_BUMPSCALING, bump_intensity) == aiReturn_SUCCESS) {
        pMaterial->SetBumpIntensity(bump_intensity);
    }

    if (material.GetTextureCount(aiTextureType_AMBIENT) > 0 && material.GetTexture(aiTextureType_AMBIENT, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, true);
        pMaterial->SetTexture(Material::TextureType::Ambient, texture);
    }

    if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0 && material.GetTexture(aiTextureType_EMISSIVE, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, true);
        pMaterial->SetTexture(Material::TextureType::Emissive, texture);
    }

    if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0 && material.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, true);
        pMaterial->SetTexture(Material::TextureType::Diffuse, texture);
    }

    if (material.GetTextureCount(aiTextureType_SPECULAR) > 0 && material.GetTexture(aiTextureType_SPECULAR, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, true);
        pMaterial->SetTexture(Material::TextureType::Specular, texture);
    }

    if (material.GetTextureCount(aiTextureType_SHININESS) > 0 && material.GetTexture(aiTextureType_SHININESS, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, false);
        pMaterial->SetTexture(Material::TextureType::SpecularPower, texture);
    }

    if (material.GetTextureCount(aiTextureType_OPACITY) > 0 && material.GetTexture(aiTextureType_OPACITY, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, false);
        pMaterial->SetTexture(Material::TextureType::Opacity, texture);
    }

    if (material.GetTextureCount(aiTextureType_NORMALS) > 0 && material.GetTexture(aiTextureType_NORMALS, 0, &aiTexture_path) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, false);
        pMaterial->SetTexture(Material::TextureType::Normal, texture);
    }
    else if (material.GetTextureCount(aiTextureType_HEIGHT) > 0 && material.GetTexture(aiTextureType_HEIGHT, 0, &aiTexture_path, nullptr, nullptr, &blend_factor) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        std::filesystem::path material_file_path = parent_path / texture_path;
        if (std::filesystem::exists(texture_path) && std::filesystem::is_regular_file(texture_path)) {
            material_file_path = texture_path;
        }
        auto texture = command_list.LoadTextureFromFile(material_file_path, false);
        Material::TextureType texture_type = (texture->BitsPerPixel() >= 24) ? Material::TextureType::Normal : Material::TextureType::Bump;
        pMaterial->SetTexture(texture_type, texture);
    }

    m_materials.push_back(pMaterial);
}

void MeshComponent::ImportMesh(CommandList& command_list, const aiMesh& ai_mesh) {
    auto mesh = std::make_shared<Mesh>();

    std::vector<VertexPositionNormalTangentBitangentTexture> vertex_data(ai_mesh.mNumVertices);

    assert(ai_mesh.mMaterialIndex < m_materials.size());
    mesh->SetMaterial(m_materials[ai_mesh.mMaterialIndex]);

    unsigned int i;
    if (ai_mesh.HasPositions()) {
        for (i = 0u; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Position = { ai_mesh.mVertices[i].x, ai_mesh.mVertices[i].y, ai_mesh.mVertices[i].z };
        }
    }

    if (ai_mesh.HasNormals()) {
        for (i = 0; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Normal = { ai_mesh.mNormals[i].x, ai_mesh.mNormals[i].y, ai_mesh.mNormals[i].z };
        }
    }

    if (ai_mesh.HasTangentsAndBitangents()) {
        for (i = 0; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Tangent = { ai_mesh.mTangents[i].x, ai_mesh.mTangents[i].y, ai_mesh.mTangents[i].z };
            vertex_data[i].Bitangent = { ai_mesh.mBitangents[i].x, ai_mesh.mBitangents[i].y, ai_mesh.mBitangents[i].z };
        }
    }

    if (ai_mesh.HasTextureCoords(0)) {
        for (i = 0; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].TexCoord = { ai_mesh.mTextureCoords[0][i].x, ai_mesh.mTextureCoords[0][i].y, ai_mesh.mTextureCoords[0][i].z };
        }
    }

    auto vertex_buffer = command_list.CopyVertexBuffer(vertex_data);
    mesh->SetVertexBuffer(0, vertex_buffer);

    if (ai_mesh.HasFaces()) {
        std::vector<unsigned int> indices;
        for (i = 0; i < ai_mesh.mNumFaces; ++i) {
            const aiFace& face = ai_mesh.mFaces[i];

            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
        }

        if (indices.size() > 0) {
            auto indexBuffer = command_list.CopyIndexBuffer(indices);
            mesh->SetIndexBuffer(indexBuffer);
        }
    }

    mesh->SetAABB(CreateBoundingBox(ai_mesh.mAABB));

    m_meshes.push_back(mesh);
}

DirectX::BoundingBox MeshComponent::CreateBoundingBox(const aiAABB& aabb) {
    DirectX::XMVECTOR min = DirectX::XMVectorSet(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z, 1.0f);
    DirectX::XMVECTOR max = DirectX::XMVectorSet(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z, 1.0f);
    
    DirectX::BoundingBox bb;
    DirectX::BoundingBox::CreateFromPoints(bb, min, max);
    
    return bb;
}

std::shared_ptr<SceneNode> MeshComponent::ImportSceneNode(CommandList& command_list, std::shared_ptr<SceneNode> parent, const aiNode* aiNode) {
    if (!aiNode) {
        return nullptr;
    }

    auto node = std::make_shared<MeshNode>(aiNode->mName.C_Str(), DirectX::XMMATRIX(&(aiNode->mTransformation.a1)));
    node->SetParent(parent);

    if (aiNode->mName.length > 0) {
        node->SetName(aiNode->mName.C_Str());
    }

    for (unsigned int i = 0; i < aiNode->mNumMeshes; ++i) {
        assert(aiNode->mMeshes[i] < m_meshes.size());

        std::shared_ptr<Mesh> pMesh = m_meshes[aiNode->mMeshes[i]];
        node->AddMesh(pMesh);
    }

    for (unsigned int i = 0; i < aiNode->mNumChildren; ++i) {
        auto child = ImportSceneNode(command_list, node, aiNode->mChildren[i]);
        node->VAddChild(child);
    }

    return node;
}
