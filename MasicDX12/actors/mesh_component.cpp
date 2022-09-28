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
#include "../nodes/mesh_node_loader.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/i_event_manager.h"
#include "transform_component.h"

#include <assimp/config.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/ProgressHandler.hpp>
#include <assimp/anim.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <unordered_map>

using MeshFileName = std::string;
using CopyCounter = int;
using NodeMap = std::unordered_map<MeshFileName, std::shared_ptr<SceneNode>>;
using CopyCounterMap = std::unordered_map<MeshFileName, CopyCounter>;

static NodeMap gs_node_map;
static CopyCounterMap gs_copy_counter_map;
const std::string MeshComponent::g_Name = "MeshComponent";

MeshComponent::MeshComponent() {}

MeshComponent::MeshComponent(const pugi::xml_node& data) {
    VDelegateInit(data);
}

MeshComponent::~MeshComponent() {
    std::shared_ptr<Actor> act = GetOwner();
    std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
    std::shared_ptr<EvtData_Destroy_Scene_Component> pNewActorEvent = std::make_shared<EvtData_Destroy_Scene_Component>(act->GetId(), VGetId(), scene_node);
    IEventManager::Get()->VQueueEvent(pNewActorEvent);

    gs_copy_counter_map[m_resource_name]--;
    assert(!(gs_copy_counter_map[m_resource_name] < 0));
    if (gs_copy_counter_map[m_resource_name] == 0) {
        gs_node_map.erase(m_resource_name);
    }
}

void MeshComponent::VDelegatePostInit() {
    std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
    scene_node->VAddChild(m_loaded_scene_node);
}

void MeshComponent::VDelegateUpdate(const GameTimerDelta& delta) {}

const std::string& MeshComponent::VGetName() const {
	return MeshComponent::g_Name;
}

pugi::xml_node MeshComponent::VGenerateXml() {
	return pugi::xml_node();
}

const std::string& MeshComponent::GetResourceName() {
    return m_resource_name;
}

const std::string& MeshComponent::GetResourceDirecory() {
    return m_resource_directory;
}

bool MeshComponent::VDelegateInit(const pugi::xml_node& data) {
	m_resource_name = data.child("Mesh").child_value();
	if (m_resource_name.empty()) return false;
	std::filesystem::path p(m_resource_name);
	m_resource_directory = p.parent_path().string();
	return LoadModel(p);
}

bool MeshComponent::LoadModel(const std::filesystem::path& file_name) {
    std::string file_path_str = file_name.string();

    if (gs_node_map.count(file_path_str)) {
        m_loaded_scene_node = DeepCopyNode(gs_node_map[file_path_str]);
        gs_copy_counter_map[file_path_str]++;
        return true;
    }

    std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(Engine::GetEngine()->GetRenderer());
    std::shared_ptr<Device> device = renderer->GetDevice();
    CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

    std::shared_ptr<SceneNode> loaded_scene = MeshNodeLoader::ImportSceneNode(*command_list, file_name);

    command_queue.ExecuteCommandList(command_list);
    command_queue.Flush();

    gs_node_map[file_path_str] = loaded_scene;
    m_loaded_scene_node = DeepCopyNode(loaded_scene);
    gs_copy_counter_map[file_path_str]++;

    return true;
}

std::shared_ptr<SceneNode> MeshComponent::DeepCopyNode(const std::shared_ptr<SceneNode>& node) {
    if (!node) return nullptr;
    std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(node);

    const auto& node_props = node->Get();
    const auto& mesh_list = mesh_node->GetMeshes();
    auto node_copy = std::make_shared<MeshNode>(node_props.Name(), node_props.ToWorld4x4(), mesh_list);

    for (const auto& child_to_copy : node->VGetChildren()) {
        auto child_to_copy_node_copy = DeepCopyNode(child_to_copy);
        node_copy->VAddChild(child_to_copy_node_copy);
        child_to_copy_node_copy->SetParent(node_copy);
    }

    return node_copy;
}