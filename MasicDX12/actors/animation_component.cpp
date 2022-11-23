#include "animation_component.h"

#include "../graphics/directx12_wrappers/command_list.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/device.h"
#include "../graphics/directx12_wrappers/texture.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"
#include "../engine/engine.h"
#include "../graphics/d3d12_renderer.h"
#include "../nodes/mesh_node.h"
#include "../nodes/aminated_mesh_node.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/i_event_manager.h"
#include "transform_component.h"
#include "../nodes/skinned_data.h"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/anim.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <cassert>
#include <unordered_map>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/anim.h>
#include <assimp/postprocess.h>

using MeshFileName = std::string;
using CopyCounter = int;
using NodeMap = std::unordered_map<MeshFileName, std::shared_ptr<SceneNode>>;
using CopyCounterMap = std::unordered_map<MeshFileName, CopyCounter>;

static NodeMap gs_node_map;
static CopyCounterMap gs_copy_counter_map;
const std::string AnimationComponent::g_Name = "AnimationComponent";
const std::filesystem::path g_textures_path("data/textures/");

using MaterialList = std::vector<std::shared_ptr<Material>>;
using MeshList = std::vector<std::shared_ptr<Mesh>>;
using BoneOffsetInfoMap = std::unordered_map<std::string, BoneInfo>;

static std::unordered_map<MeshFileName, std::vector<std::shared_ptr<AnimatedMeshNode>>> gs_mesh_nodes_list;
static std::unordered_map<MeshFileName, std::shared_ptr<AnimatedMeshNode::FinalTransformList>> gs_mesh_nodes_final_list;

constexpr unsigned int MAX_BONE_WEIGHTS = 4u;
constexpr unsigned int EMPTY_BONE = 255u;

struct NodeHeirarchyData {
    DirectX::XMFLOAT4X4 transformation;
    std::string name;
    std::vector<NodeHeirarchyData> children;
};

DirectX::BoundingBox CreateBoundingBox2(const aiAABB& aabb) {
    DirectX::XMVECTOR min = DirectX::XMVectorSet(aabb.mMin.x, aabb.mMin.y, aabb.mMin.z, 1.0f);
    DirectX::XMVECTOR max = DirectX::XMVectorSet(aabb.mMax.x, aabb.mMax.y, aabb.mMax.z, 1.0f);

    DirectX::BoundingBox bb;
    DirectX::BoundingBox::CreateFromPoints(bb, min, max);

    return bb;
}

void ChangeBoneWeight(DirectX::XMFLOAT3& bone_weights, size_t index, float weight) {
    switch (index) {
    case 0u: bone_weights.x = weight; break;
    case 1u: bone_weights.y = weight; break;
    case 2u: bone_weights.z = weight; break;
    default:
        break;
    }
}

void AddVertexBoneInfo(VertexPosNormTgBtgUVAnim& vert, float vertex_weight, BYTE new_bone_index) {
    for (unsigned int k = 0u; k < MAX_BONE_WEIGHTS; ++k) {
        auto current_bone_index = vert.BoneIndices[k];
        if (current_bone_index == EMPTY_BONE) {
            ChangeBoneWeight(vert.BoneWeights, k, vertex_weight);
            vert.BoneIndices[k] = new_bone_index;
            break;
        }
    }
}

void ImportMesh2(MeshList& mesh_list, CommandList& command_list, const aiMesh& ai_mesh, const BoneOffsetInfoMap& bone_offset_info_map) {
    auto mesh = std::make_shared<Mesh>();

    if (ai_mesh.mName.length > 0u) {
        mesh->SetName(ai_mesh.mName.C_Str());
    }

    std::vector<VertexPosNormTgBtgUVAnim> vertex_data(ai_mesh.mNumVertices);
    if (ai_mesh.HasPositions()) {
        for (unsigned int i = 0u; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Position = { ai_mesh.mVertices[i].x, ai_mesh.mVertices[i].y, ai_mesh.mVertices[i].z };
        }
    }

    if (ai_mesh.HasNormals()) {
        for (unsigned int i = 0u; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Normal = { ai_mesh.mNormals[i].x, ai_mesh.mNormals[i].y, ai_mesh.mNormals[i].z };
        }
    }

    if (ai_mesh.HasTangentsAndBitangents()) {
        for (unsigned int i = 0u; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].Tangent = { ai_mesh.mTangents[i].x, ai_mesh.mTangents[i].y, ai_mesh.mTangents[i].z };
            vertex_data[i].Bitangent = { ai_mesh.mBitangents[i].x, ai_mesh.mBitangents[i].y, ai_mesh.mBitangents[i].z };
        }
    }

    if (ai_mesh.HasTextureCoords(0)) {
        for (unsigned int i = 0u; i < ai_mesh.mNumVertices; ++i) {
            vertex_data[i].TexCoord = { ai_mesh.mTextureCoords[0][i].x, ai_mesh.mTextureCoords[0][i].y, ai_mesh.mTextureCoords[0][i].z };
        }
    }

    if (ai_mesh.HasBones()) {
        size_t vertex_sz = vertex_data.size();
        for (unsigned int j = 0u; j < vertex_sz; ++j) {
            vertex_data[j].BoneWeights = {0.0f, 0.0f, 0.0f};
            vertex_data[j].BoneIndices[0] = EMPTY_BONE;
            vertex_data[j].BoneIndices[1] = EMPTY_BONE;
            vertex_data[j].BoneIndices[2] = EMPTY_BONE;
            vertex_data[j].BoneIndices[3] = EMPTY_BONE;
        }
        for (unsigned int i = 0u; i < ai_mesh.mNumBones; ++i) {
            const aiBone& ai_bone = *(ai_mesh.mBones[i]);
            std::string bone_name(ai_bone.mName.C_Str());
            const auto& bone_offset_info = bone_offset_info_map.at(bone_name);
            //uint32_t sz = std::min(MAX_BONE_WEIGHTS, ai_bone.mNumWeights);
            //uint32_t sz_remainder = MAX_BONE_WEIGHTS - sz;
            uint32_t sz = ai_bone.mNumWeights;
            for (unsigned int j = 0u; j < sz; ++j) {
                const aiVertexWeight& ai_vertex_weight = ai_bone.mWeights[j];
                int vertex_id = ai_vertex_weight.mVertexId;
                float vertex_weight = ai_vertex_weight.mWeight;
                if (vertex_weight == 0.0f) continue;
                auto& vert = vertex_data[vertex_id];
                AddVertexBoneInfo(vert, vertex_weight, bone_offset_info.id);
            }
        }
        for (unsigned int j = 0u; j < vertex_sz; ++j) {
            for (unsigned int k = 0u; k < MAX_BONE_WEIGHTS; ++k) {
                if (vertex_data[j].BoneIndices[k] == EMPTY_BONE) {
                    ChangeBoneWeight(vertex_data[j].BoneWeights, k, 0.0f);
                    vertex_data[j].BoneIndices[k] = 0u;
                }
            }
        }
    }

    auto vertex_buffer = command_list.CopyVertexBuffer(vertex_data);
    mesh->SetVertexBuffer(0u, vertex_buffer);

    if (ai_mesh.HasFaces()) {
        std::vector<unsigned int> indices;
        size_t sz = ai_mesh.mNumFaces * ai_mesh.mFaces[0u].mNumIndices;
        indices.reserve(sz);
        for (unsigned int i = 0u; i < ai_mesh.mNumFaces; ++i) {
            const aiFace& face = ai_mesh.mFaces[i];

            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
            }
            else if (face.mNumIndices == 4) {
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[1]);
                indices.push_back(face.mIndices[2]);
                indices.push_back(face.mIndices[0]);
                indices.push_back(face.mIndices[2]);
                indices.push_back(face.mIndices[3]);
            }
        }

        if (indices.size() > 0u) {
            auto indexBuffer = command_list.CopyIndexBuffer(indices);
            mesh->SetIndexBuffer(indexBuffer);
        }
    }

    mesh->SetAABB(CreateBoundingBox2(ai_mesh.mAABB));

    mesh_list.push_back(mesh);
}

void ImportMesh2(MeshList& mesh_list, MaterialList& material_list, CommandList& command_list, const aiScene& scene, const BoneOffsetInfoMap& bone_offset_info_map) {
    for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
        ImportMesh2(mesh_list, command_list, *(scene.mMeshes[i]), bone_offset_info_map);

        unsigned int mat_index = (*(scene.mMeshes[i])).mMaterialIndex;
        assert(mat_index < material_list.size());
        mesh_list.back()->SetMaterial(material_list[mat_index]);
    }
}

void ImportBoneOffsetInfo2(BoneOffsetInfoMap& bone_offset_info_map, const aiScene& scene) {
    for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
        const aiMesh& ai_mesh = *(scene.mMeshes[i]);
        std::string mesh_name(ai_mesh.mName.C_Str());
        for (unsigned int i = 0u; i < ai_mesh.mNumBones; ++i) {
            const aiBone& ai_bone = *ai_mesh.mBones[i];
            std::string bone_name(ai_bone.mName.C_Str());
            
            //DirectX::XMFLOAT4X4 offset_matrix(&(ai_bone.mOffsetMatrix.a1));
            DirectX::XMFLOAT4X4 offset_matrix = {};
            DirectX::XMMATRIX offset_matrixXM(&(ai_bone.mOffsetMatrix.a1));
            DirectX::XMStoreFloat4x4(&offset_matrix, DirectX::XMMatrixTranspose(offset_matrixXM));
            if (!bone_offset_info_map.count(bone_name)) {
                bone_offset_info_map[bone_name].id = bone_offset_info_map.size();
            }
            bone_offset_info_map[bone_name].offset = offset_matrix;
        }
    }
}

std::shared_ptr<Texture> GetTexture(CommandList& command_list, std::filesystem::path file_name, std::filesystem::path parent_path) {
    std::filesystem::path texture_path(file_name);
    std::filesystem::path texture_file_name = texture_path.filename();

    std::filesystem::path material_file_path = texture_path;
    bool file_exists = std::filesystem::exists(material_file_path) && std::filesystem::is_regular_file(material_file_path);

    if (!file_exists) {
        std::filesystem::path parent_file_path = parent_path / texture_path;
        file_exists = std::filesystem::exists(parent_file_path) && std::filesystem::is_regular_file(parent_file_path);
        if (file_exists) {
            material_file_path = parent_file_path;
        }
    }

    if (!file_exists) {
        std::filesystem::path special_texture_path = g_textures_path / texture_file_name;
        file_exists = std::filesystem::exists(special_texture_path) && std::filesystem::is_regular_file(special_texture_path);
        if (file_exists) {
            material_file_path = special_texture_path;
        }
    }

    std::shared_ptr<Texture> texture = nullptr;
    if (file_exists) {
        texture = command_list.LoadTextureFromFile(material_file_path, true);
    }

    return texture;
}

void ImportMaterial2(MaterialList& material_list, CommandList& command_list, const aiMaterial& material, std::filesystem::path parent_path, bool is_inv_y_texture) {
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
    pMaterial->SetInvYNormalTextureFlag(is_inv_y_texture);

    aiString mat_name = material.GetName();
    if (mat_name.length > 0u) {
        pMaterial->SetName(mat_name.C_Str());
    }

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
    if (material.Get(AI_MATKEY_REFRACTI, index_of_refraction) == aiReturn_SUCCESS) {
        pMaterial->SetIndexOfRefraction(index_of_refraction);
    }
    if (material.Get(AI_MATKEY_REFLECTIVITY, reflectivity) == aiReturn_SUCCESS) {
        pMaterial->SetReflectance(DirectX::XMFLOAT4(reflectivity, reflectivity, reflectivity, reflectivity));
    }
    if (material.Get(AI_MATKEY_BUMPSCALING, bump_intensity) == aiReturn_SUCCESS) {
        pMaterial->SetBumpIntensity(bump_intensity);
    }

    if (material.GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0 && material.GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Ambient, texture);
    }
    else if (material.GetTextureCount(aiTextureType_AMBIENT) > 0 && material.GetTexture(aiTextureType_AMBIENT, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Ambient, texture);
    }

    if (material.GetTextureCount(aiTextureType_EMISSIVE) > 0 && material.GetTexture(aiTextureType_EMISSIVE, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Emissive, texture);
    }

    if (material.GetTextureCount(aiTextureType_BASE_COLOR) > 0 && material.GetTexture(aiTextureType_BASE_COLOR, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Diffuse, texture);
    }
    else if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0 && material.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Diffuse, texture);
    }

    if (material.GetTextureCount(aiTextureType_SPECULAR) > 0 && material.GetTexture(aiTextureType_SPECULAR, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Specular, texture);
    }

    if (material.GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0 && material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::SpecularPower, texture);
    }
    else if (material.GetTextureCount(aiTextureType_SHININESS) > 0 && material.GetTexture(aiTextureType_SHININESS, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::SpecularPower, texture);
    }

    if (material.GetTextureCount(aiTextureType_OPACITY) > 0 && material.GetTexture(aiTextureType_OPACITY, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Opacity, texture);
    }

    if (material.GetTextureCount(aiTextureType_NORMALS) > 0 && material.GetTexture(aiTextureType_NORMALS, 0, &aiTexture_path) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Normal, texture);
    }
    else if (material.GetTextureCount(aiTextureType_HEIGHT) > 0 && material.GetTexture(aiTextureType_HEIGHT, 0, &aiTexture_path, nullptr, nullptr, &blend_factor) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        Material::TextureType texture_type = (texture->BitsPerPixel() >= 24) ? Material::TextureType::Normal : Material::TextureType::Bump;
        pMaterial->SetTexture(texture_type, texture);
    }

    if (material.GetTextureCount(aiTextureType_DISPLACEMENT) > 0 && material.GetTexture(aiTextureType_DISPLACEMENT, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Displacement, texture);
    }

    if (material.GetTextureCount(aiTextureType_METALNESS) > 0 && material.GetTexture(aiTextureType_METALNESS, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Metalness, texture);
    }
    else if (material.GetTextureCount(aiTextureType_REFLECTION) > 0 && material.GetTexture(aiTextureType_REFLECTION, 0, &aiTexture_path, nullptr, nullptr, &blend_factor, &aiBlend_operation) == aiReturn_SUCCESS) {
        std::filesystem::path texture_path(aiTexture_path.C_Str());
        auto texture = GetTexture(command_list, texture_path, parent_path);;
        pMaterial->SetTexture(Material::TextureType::Metalness, texture);
    }

    material_list.push_back(pMaterial);
}

void ImportMaterial2(MaterialList& material_list, CommandList& command_list, const aiScene& scene, std::filesystem::path parent_path, bool is_inv_y_texture) {
    for (unsigned int i = 0u; i < scene.mNumMaterials; ++i) {
        ImportMaterial2(material_list, command_list, *(scene.mMaterials[i]), parent_path, is_inv_y_texture);
    }
}

bool CheckForMesh(const aiNode* ai_node) {
    if (ai_node->mNumMeshes) return true;
    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        const aiNode* child = ai_node->mChildren[i];
        if (child->mNumMeshes > 0u) return true;
        bool deep = CheckForMesh(child);
        if (deep == true) return true;
    }
    return false;
}

std::shared_ptr<SceneNode> ImportSceneNode2(MeshList mesh_list, std::shared_ptr<SceneNode> parent, const aiNode* ai_node, const std::string& file_name) {
    if (!ai_node) return nullptr;

    std::string node_name(ai_node->mName.C_Str());

    std::shared_ptr<SceneNode> node = nullptr;

    DirectX::XMMATRIX transform(&(ai_node->mTransformation.a1));
    transform = DirectX::XMMatrixTranspose(transform);

    if (ai_node->mNumMeshes) {
        std::shared_ptr<AnimatedMeshNode> mesh_node = std::make_shared<AnimatedMeshNode>(file_name + node_name, transform);
        mesh_node->SetParent(parent);

        for (unsigned int i = 0; i < ai_node->mNumMeshes; ++i) {
            assert(ai_node->mMeshes[i] < mesh_list.size());

            std::shared_ptr<Mesh> pMesh = mesh_list[ai_node->mMeshes[i]];
            mesh_node->AddMesh(pMesh);
        }
        node = mesh_node;
    }
    else {
        node = std::make_shared<SceneNode>(file_name + node_name, transform);
        node->SetParent(parent);
    }

    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        const aiNode* child_ai_node = ai_node->mChildren[i];
        if (!CheckForMesh(child_ai_node)) continue;
        
        auto child = ImportSceneNode2(mesh_list, node, child_ai_node, file_name);
        if (child) node->VAddChild(child);
    }

    return node;
}

void ImportBoneNames2(std::unordered_set<std::string>& bone_names, const aiScene& scene) {
    for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
        const aiMesh& ai_mesh = *(scene.mMeshes[i]);
        std::string mesh_name(ai_mesh.mName.C_Str());
        for (unsigned int i = 0u; i < ai_mesh.mNumBones; ++i) {
            const aiBone& ai_bone = *ai_mesh.mBones[i];
            std::string bone_name(ai_bone.mName.C_Str());
            bone_names.emplace(std::move(bone_name));
        }
    }
}


void ImportBoneOffsetInfoMapKeys(const aiNode* current_aiNode, const std::unordered_set<std::string>& bone_names, bool in_bone_struct, BoneOffsetInfoMap& bone_offset_info_map) {
    if (!current_aiNode) return;

    std::string current_node_name(current_aiNode->mName.C_Str());

    in_bone_struct |= bone_names.count(current_node_name);

    if (in_bone_struct) {
        BoneInfo bone_info = {};
        bone_info.id = bone_offset_info_map.size();
        DirectX::XMStoreFloat4x4(&bone_info.offset, DirectX::XMMatrixIdentity());
        bone_offset_info_map[current_node_name] = bone_info;
    }

    for (unsigned int i = 0; i < current_aiNode->mNumChildren; ++i) {
        ImportBoneOffsetInfoMapKeys(current_aiNode->mChildren[i], bone_names, in_bone_struct, bone_offset_info_map);
    }
}

void ImportBoneHierarchy(const aiNode* parent_aiNode, const aiNode* current_aiNode, const BoneOffsetInfoMap& bone_offset_info_map, std::vector<int>& hierarchy) {
    if (!current_aiNode) return;

    std::string current_node_name(current_aiNode->mName.C_Str());

    bool current_node_is_bone = bone_offset_info_map.count(current_node_name);
    if (current_node_is_bone && parent_aiNode) {
        std::string parent_node_name(parent_aiNode->mName.C_Str());
        bool parent_node_is_bone = bone_offset_info_map.count(parent_node_name);
        if (parent_node_is_bone) {
            const BoneInfo& current_bone_info = bone_offset_info_map.at(current_node_name);
            const BoneInfo& parent_bone_info = bone_offset_info_map.at(parent_node_name);
            hierarchy[current_bone_info.id] = parent_bone_info.id;
        }
    }

    for (unsigned int i = 0; i < current_aiNode->mNumChildren; ++i) {
        ImportBoneHierarchy(current_aiNode, current_aiNode->mChildren[i], bone_offset_info_map, hierarchy);
    }
}


const aiNode* GetRootSceneNode(const aiNode* ai_current_node, const aiNode* ai_parent_node) {
    if (ai_current_node->mNumMeshes > 0u) return ai_parent_node;
    bool have_mesh_child = false;
    for (unsigned int i = 0; i < ai_current_node->mNumChildren; ++i) {
        const aiNode* child = ai_current_node->mChildren[i];
        if (child->mNumMeshes > 0u) {
            have_mesh_child = true;
            break;
        }
    }
    if (have_mesh_child) return ai_current_node;
    for (unsigned int i = 0; i < ai_current_node->mNumChildren; ++i) {
        const aiNode* child = ai_current_node->mChildren[i];
        const aiNode* root = GetRootSceneNode(child, ai_current_node);
        if (root) return root;
    }
    return nullptr;
}

const aiNode* GetRootSceneNode(const aiNode* ai_node) {
    if (ai_node->mNumMeshes > 0u) return ai_node;
    bool have_mesh_child = false;
    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        const aiNode* child = ai_node->mChildren[i];
        if (child->mNumMeshes > 0u) {
            have_mesh_child = true;
            break;
        }
    }
    if (have_mesh_child) return ai_node;
    for (unsigned int i = 0; i < ai_node->mNumChildren; ++i) {
        const aiNode* child = ai_node->mChildren[i];
        const aiNode* root = GetRootSceneNode(child, ai_node);
        if (root) return root;
    }
    return ai_node;
}

DirectX::XMMATRIX GetMatrixOnTop(const aiNode* ai_current_node) {
    DirectX::XMMATRIX matrix = DirectX::XMMatrixIdentity();
    if (!ai_current_node) return matrix;
    std::vector<DirectX::XMMATRIX> matrices;
    while (ai_current_node) {
        DirectX::XMMATRIX transform(&(ai_current_node->mTransformation.a1));
        transform = DirectX::XMMatrixTranspose(transform);
        DirectX::XMVECTOR scale;
        DirectX::XMVECTOR rotation;
        DirectX::XMVECTOR translation;
        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, transform);
        DirectX::XMMATRIX to_worlad = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationQuaternion(rotation), DirectX::XMMatrixTranslationFromVector(translation));

        matrices.push_back(to_worlad);

        ai_current_node = ai_current_node->mParent;
    }

    return matrix;
}

std::shared_ptr<SceneNode> AnimationComponent::ImportScene2(CommandList& command_list, const aiScene& scene, std::filesystem::path parent_path, const std::string& file_name, bool is_inv_y_texture) {
    MaterialList material_list;
    MeshList mesh_list;
    BoneOffsetInfoMap bone_offset_info_map;
    std::unordered_set<std::string> bone_names;

    ImportMaterial2(material_list, command_list, scene, parent_path, is_inv_y_texture);
    ImportBoneNames2(bone_names, scene);
    bool in_bone_struct = bone_names.count(std::string(scene.mRootNode->mName.C_Str()));
    ImportBoneOffsetInfoMapKeys(scene.mRootNode, bone_names, in_bone_struct, bone_offset_info_map);
    ImportBoneOffsetInfo2(bone_offset_info_map, scene);
    
    if (scene.hasSkeletons()) {
        SkinnedData sd;
        for (unsigned int i = 0; i < scene.mNumSkeletons; ++i) {
            const aiSkeleton skeleton =(*scene.mSkeletons[i]);
            std::string skeleton_name(skeleton.mName.C_Str());
        }
    }

    if (scene.HasAnimations()) {
        int bone_counter = bone_offset_info_map.size();
        m_skinned_data = std::make_shared<SkinnedData>();

        for (unsigned int i = 0; i < scene.mNumAnimations; ++i) {
            const aiAnimation& animation = *(scene.mAnimations[i]);
            std::string animation_name(animation.mName.C_Str());

            float tick_time_sec = 1.0f / animation.mTicksPerSecond;
            float duration_sec = animation.mDuration * tick_time_sec;

            AnimationClip ac;
            ac.KeyframesMap.reserve(animation.mNumChannels);
            //ac.BoneAnimations.reserve(animation.mNumChannels);
            ac.BoneAnimations = std::vector<BoneAnimationPtr>(bone_offset_info_map.size(), nullptr);
            for (unsigned int i = 0; i < animation.mNumChannels; ++i) {
                const aiNodeAnim& animation_channel = *(animation.mChannels[i]);
                std::string channel_name(animation_channel.mNodeName.C_Str());
                const auto& bone_info = bone_offset_info_map.at(channel_name);

                if (!bone_offset_info_map.count(channel_name)) {
                    DirectX::XMFLOAT4X4 identity_offset_matrix;
                    DirectX::XMStoreFloat4x4(&identity_offset_matrix, DirectX::XMMatrixIdentity());
                    BoneInfo bone_info = { bone_counter, identity_offset_matrix };
                    bone_offset_info_map[channel_name] = std::move(bone_info);
                    ++bone_counter;
                }

                std::vector<KeyframeTranslation> translation_key_frames;
                translation_key_frames.reserve(animation_channel.mNumPositionKeys);
                for (unsigned int i = 0; i < animation_channel.mNumPositionKeys; ++i) {
                    KeyframeTranslation kf;
                    const aiVectorKey& animation_translation = animation_channel.mPositionKeys[i];
                    float time_in_ticks = animation_translation.mTime;

                    kf.TimePos = time_in_ticks * tick_time_sec;
                    kf.Translation = DirectX::XMFLOAT3(&(animation_translation.mValue.x));
                    translation_key_frames.push_back(kf);
                }
                
                std::vector<KeyframeScale> scale_key_frames;
                scale_key_frames.reserve(animation_channel.mNumScalingKeys);
                for (unsigned int i = 0; i < animation_channel.mNumScalingKeys; ++i) {
                    KeyframeScale kf;
                    const aiVectorKey& animation_scale = animation_channel.mScalingKeys[i];
                    float time_in_ticks = animation_scale.mTime;

                    kf.TimePos = time_in_ticks * tick_time_sec;
                    kf.Scale = DirectX::XMFLOAT3(&(animation_scale.mValue.x));
                    scale_key_frames.push_back(kf);
                }

                std::vector<KeyframeRotation> rotation_key_frames;
                rotation_key_frames.reserve(animation_channel.mNumRotationKeys);
                for (unsigned int i = 0; i < animation_channel.mNumRotationKeys; ++i) {
                    KeyframeRotation kf;
                    const aiQuatKey& animation_rotation = animation_channel.mRotationKeys[i];
                    float time_in_ticks = animation_rotation.mTime;

                    kf.TimePos = time_in_ticks * tick_time_sec;
                    kf.RotationQuat.x = animation_rotation.mValue.x;
                    kf.RotationQuat.y = animation_rotation.mValue.y;
                    kf.RotationQuat.z = animation_rotation.mValue.z;
                    kf.RotationQuat.w = animation_rotation.mValue.w;
                    rotation_key_frames.push_back(kf);
                }

                BoneAnimationPtr pBoneAnimation = std::make_shared<BoneAnimation>();
                pBoneAnimation->TranslationKeyframes = std::move(translation_key_frames);
                pBoneAnimation->ScaleKeyframes = std::move(scale_key_frames);
                pBoneAnimation->RotationKeyframes = std::move(rotation_key_frames);

                ac.KeyframesMap[channel_name] = pBoneAnimation;
                //ac.BoneAnimations.push_back(pBoneAnimation);

                //ac.BoneAnimations = std::vector<BoneAnimationPtr>(bone_offset_info_map.size(), nullptr);
                size_t bone_index = bone_info.id;
                ac.BoneAnimations[bone_index] = pBoneAnimation;
            }
            m_skinned_data->AddAnimations(animation_name, std::move(ac));
        }
        std::vector<int> bone_hierarchy = std::vector<int>(bone_counter, 0);
        ImportBoneHierarchy(nullptr, scene.mRootNode, bone_offset_info_map, bone_hierarchy);
        m_skinned_data->SetBoneHierarchy(std::move(bone_hierarchy));

        DirectX::XMFLOAT4X4 identity_matrix = {};
        DirectX::XMStoreFloat4x4(&identity_matrix, DirectX::XMMatrixIdentity());
        std::vector<DirectX::XMFLOAT4X4> bone_offsets(bone_counter, identity_matrix);
        for (auto const&[name, bone_info] : bone_offset_info_map) {
            bone_offsets[bone_info.id] = bone_info.offset;
        }
        m_skinned_data->SetBoneOffsets(std::move(bone_offsets));
    }

    ImportMesh2(mesh_list, material_list, command_list, scene, bone_offset_info_map);
    m_skinned_data->SetBoneOffsetsMap(std::move(bone_offset_info_map));
    for (unsigned int i = 0; i < scene.mNumMeshes; ++i) {
        mesh_list[i]->SetSkinnedData(m_skinned_data);
    }

    /*const aiNode* ai_root_node = scene.mRootNode;
    if (ai_root_node->mNumMeshes == 0u) {
        while(ai_root_node)
        bool childs_have_mesh = false;
        for (unsigned int i = 0; i < ai_root_node->mNumChildren; ++i) {
            const aiNode* child = ai_node->mChildren[i];
            if (child->mNumMeshes > 0u) return true;
            bool deep = CheckForMesh(child);
            if (deep == true) return true;
        }
        return false;
    }*/
    const aiNode* ai_root_node = GetRootSceneNode(scene.mRootNode);
    std::shared_ptr<SceneNode> root_node = ImportSceneNode2(mesh_list, nullptr, ai_root_node, file_name);
    
    return root_node;
}

std::shared_ptr<SceneNode> AnimationComponent::ImportSceneNode(CommandList& command_list, const std::filesystem::path& file_name, bool is_inv_y_texture) {

    std::filesystem::path file_path = file_name;
    std::filesystem::path export_path = std::filesystem::path(file_path).replace_extension("assbin");

    std::filesystem::path parent_path;
    if (file_path.has_parent_path()) {
        parent_path = file_path.parent_path();
    }
    else {
        parent_path = std::filesystem::current_path();
    }

    std::string file_path_str = file_path.string();

    Assimp::Importer importer;
    const aiScene* scene;

    if (std::filesystem::exists(export_path) && std::filesystem::is_regular_file(export_path)) {
        scene = importer.ReadFile(export_path.string(), aiProcess_GenBoundingBoxes);
    }
    else {
        importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

        //unsigned int preprocess_flags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_OptimizeGraph | aiProcess_ConvertToLeftHanded | aiProcess_GenBoundingBoxes;
        unsigned int preprocess_flags = aiProcess_ConvertToLeftHanded | aiProcess_GenBoundingBoxes | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights | aiProcess_Triangulate;
        scene = importer.ReadFile(file_path.string(), preprocess_flags);

        if (scene) {
            Assimp::Exporter exporter;
            exporter.Export(scene, "assbin", export_path.string(), 0);
        }
    }

    if (!scene) {
        return std::shared_ptr<SceneNode>();
    }

    return ImportScene2(command_list, *scene, parent_path, file_path_str, is_inv_y_texture);
}

AnimationComponent::AnimationComponent() {}

AnimationComponent::AnimationComponent(const pugi::xml_node& data) {
    VDelegateInit(data);
}

AnimationComponent::~AnimationComponent() {
    std::shared_ptr<Actor> act = GetOwner();
    std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
    std::shared_ptr<EvtData_Destroy_Scene_Component> pDestroyActorComponentEvent = std::make_shared<EvtData_Destroy_Scene_Component>(act->GetId(), VGetId(), scene_node);
    IEventManager::Get()->VQueueEvent(pDestroyActorComponentEvent);

    gs_copy_counter_map[m_resource_name]--;
    assert(!(gs_copy_counter_map[m_resource_name] < 0));
    if (gs_copy_counter_map[m_resource_name] == 0) {
        gs_node_map.erase(m_resource_name);
        gs_mesh_nodes_list.erase(m_resource_name);
        gs_mesh_nodes_final_list.erase(m_resource_name);
    }
}

void AnimationComponent::VDelegatePostInit() {
    std::shared_ptr<Actor> act = GetOwner();
    std::string name = act->GetName() + "-AnimationComponent"s;
    std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
    scene_node->SetName(name);

    //std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
    scene_node->VAddChild(m_loaded_scene_node);
}

void AnimationComponent::VDelegateUpdate(const GameTimerDelta& delta) {
    for (auto animated_node : gs_mesh_nodes_list[m_resource_name]) {
        auto& final_transform_list = animated_node->GetFinalTransformList();
        float time = std::fmodf(delta.fGetTotalSeconds(), 1.0f);
        std::string clip_name = m_skinned_data->GetAnimations().cbegin()->first;
        m_skinned_data->GetFinalTransforms(clip_name, time, final_transform_list);
    }
    if (m_current_gen_updated) {
        m_loaded_scene_node->AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Mesh));
    }
}

const std::string& AnimationComponent::VGetName() const {
    return AnimationComponent::g_Name;
}

pugi::xml_node AnimationComponent::VGenerateXml() {
    return pugi::xml_node();
}

const std::string& AnimationComponent::GetResourceName() {
    return m_resource_name;
}

const std::string& AnimationComponent::GetResourceDirecory() {
    return m_resource_directory;
}

bool AnimationComponent::VDelegateInit(const pugi::xml_node& data) {
    bool is_instanced = data.child("IsInstanced").text().as_bool();
    bool is_inv_y_texture = data.child("IsInvYNormalTexture").text().as_bool();
    m_resource_name = data.child("Mesh").child_value();
    if (m_resource_name.empty()) return false;
    std::filesystem::path p(m_resource_name);
    m_resource_directory = p.parent_path().string();
    return LoadModel(p, is_instanced, is_inv_y_texture);
}

bool AnimationComponent::LoadModel(const std::filesystem::path& file_name, bool is_instanced, bool is_inv_y_texture) {
    std::string file_path_str = file_name.string();

    if (gs_node_map.count(file_path_str)) {
        m_loaded_scene_node = DeepCopyNode(gs_node_map[file_path_str], is_instanced);
        std::shared_ptr<AnimatedMeshNode> anim_mesh_node = std::dynamic_pointer_cast<AnimatedMeshNode>(m_loaded_scene_node);
        anim_mesh_node->SetIsInstanced(is_instanced);
        gs_copy_counter_map[file_path_str]++;
        return true;
    }

    std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(Engine::GetEngine()->GetRenderer());
    std::shared_ptr<Device> device = renderer->GetDevice();
    CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

    std::shared_ptr<SceneNode> loaded_scene = ImportSceneNode(*command_list, file_name, is_inv_y_texture);

    command_queue.ExecuteCommandList(command_list);
    command_queue.Flush();

    gs_node_map[file_path_str] = loaded_scene;
    m_loaded_scene_node = DeepCopyNode(loaded_scene, is_instanced);
    gs_copy_counter_map[file_path_str]++;

    return true;
}

std::shared_ptr<SceneNode> AnimationComponent::DeepCopyNode(const std::shared_ptr<SceneNode>& node, bool is_instanced) {
    if (!node) return nullptr;
    const auto& node_props = node->Get();
    std::shared_ptr<SceneNode> node_copy = nullptr;
    std::shared_ptr<AnimatedMeshNode> mesh_node = std::dynamic_pointer_cast<AnimatedMeshNode>(node);
    if (!mesh_node) {
        node_copy = std::make_shared<SceneNode>(node_props.Name(), &node_props.ToParent4x4());
    }
    else {
        const auto& mesh_list = mesh_node->GetMeshes();
        std::shared_ptr<AnimatedMeshNode> temp_node_copy = std::make_shared<AnimatedMeshNode>(node_props.Name(), node_props.ToParent4x4(), mesh_list);
        temp_node_copy->SetIsInstanced(is_instanced);
        gs_mesh_nodes_list[m_resource_name].push_back(temp_node_copy);

        if (!gs_mesh_nodes_final_list.count(m_resource_name)) {
            size_t sz = 96u;
            DirectX::XMFLOAT4X4 itentity_matrix = {};
            DirectX::XMStoreFloat4x4(&itentity_matrix, DirectX::XMMatrixIdentity());
            std::shared_ptr<std::vector<DirectX::XMFLOAT4X4>> final_transforms_def = std::make_shared<std::vector<DirectX::XMFLOAT4X4>>(sz, itentity_matrix);
            gs_mesh_nodes_final_list[m_resource_name] = final_transforms_def;
        }

        temp_node_copy->SetFinalTransformList(gs_mesh_nodes_final_list[m_resource_name]);

        node_copy = temp_node_copy;
    }

    for (const auto& child_to_copy : node->VGetChildren()) {
        auto child_to_copy_node_copy = DeepCopyNode(child_to_copy, is_instanced);
        node_copy->VAddChild(child_to_copy_node_copy);
        child_to_copy_node_copy->SetParent(node_copy);
    }

    return node_copy;
}