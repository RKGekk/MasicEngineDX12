#include "mesh_manager.h"

#include "camera_node.h"
#include "basic_camera_node.h"
#include "mesh_node.h"
#include "qualifier_node.h"
#include "scene_node.h"
#include "../engine/engine.h"
#include "../graphics/d3d12_renderer.h"
#include "../graphics/directx12_wrappers/command_queue.h"
#include "../graphics/directx12_wrappers/shader_resource_view.h"
#include "../graphics/directx12_wrappers/structured_buffer.h"
#include "../graphics/directx12_wrappers/constant_buffer.h"

#include <utility>
#include <numeric>

MeshManager::MeshManager() {}

void MeshManager::AddMesh(std::shared_ptr<SceneNode> node) {
	using namespace DirectX;
	if (!node) return;
	std::shared_ptr<MeshNode> mesh_node = std::dynamic_pointer_cast<MeshNode>(node);
	if (mesh_node) {
		const std::string& name = mesh_node->Get().Name();
		auto &mesh_list = m_mesh_map[name];
		MeshList::iterator iter = std::find(mesh_list.begin(), mesh_list.end(), mesh_node);
		if (iter == mesh_list.end()) {
			mesh_list.push_back(mesh_node);
			const auto& props = mesh_node->Get();
			InstanceData id = {};
			id.World = props.FullCumulativeToWorld4x4T();
			XMMATRIX world = XMLoadFloat4x4(&id.World);
			XMMATRIX inv_world_t = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
			XMStoreFloat4x4(&id.InverseTransposeWorld, inv_world_t);
			XMStoreFloat4x4(&id.TexureUVTransform, XMMatrixIdentity());
			m_instance_map[name].push_back(id);
			ManageInstanceBufferMap(name);
		}
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
		auto mesh_list = m_mesh_map[name];
		MeshList::iterator iter = std::find(mesh_list.begin(), mesh_list.end(), mesh_node);
		if (iter != mesh_list.end()) {
			auto &mesh_instance_data_list = m_instance_map[name];
			const auto dist = std::distance(mesh_list.begin(), iter);
			auto instance_iter = mesh_instance_data_list.begin() + dist;
			mesh_instance_data_list.erase(instance_iter);
			m_mesh_map[name].erase(iter);
			ManageInstanceBufferMap(name);
		}
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
	MeshName mesh_name = node->Get().Name();
	return m_mesh_map.count(mesh_name);
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

void MeshManager::CalcInstances(const BasicCameraNode& camera) {
	using namespace DirectX;
	BoundingFrustum world_space_frustum = camera.GetFrustum();
	for (const auto& [key, value] : m_mesh_map) {
		auto& index_list = m_instance_index_map[key];
		index_list.clear();

		const auto& mesh_list = value;
		int sz = mesh_list.size();
		for (int i = 0; i < sz; ++i) {
			const auto& mesh = mesh_list[i];
			
			const BoundingBox& aabb = mesh->Get().MergedAABB();
			
			bool frust_conatains_mesh = world_space_frustum.Contains(aabb) != DirectX::DISJOINT;
			if (frust_conatains_mesh) {
				InstanceIndexData mid = {0};
				mid.InstanceIndex = i;
				index_list.push_back(mid);
			}
		}
	}
}

void MeshManager::UpdateInstancesBuffer() {
	using namespace DirectX;
	for (const auto& [name, mesh_list] : m_mesh_map) {
		int sz = mesh_list.size();
		for (int i = 0; i < sz; ++i) {
			const auto& mesh_node = mesh_list[i];
			const auto& props = mesh_node->Get();
			InstanceData id = {};
			id.World = props.FullCumulativeToWorld4x4T();
			XMMATRIX world = XMLoadFloat4x4(&id.World);
			XMMATRIX inv_world_t = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
			XMStoreFloat4x4(&id.InverseTransposeWorld, inv_world_t);
			XMStoreFloat4x4(&id.TexureUVTransform, XMMatrixIdentity());
			m_instance_map[name][i] = id;
		}
		ManageInstanceBufferMap(name);
	}
}

MeshManager::InstanceMap& MeshManager::GetInstanceMap() {
	return m_instance_map;
}

std::vector<MeshManager::InstanceData>& MeshManager::GetInstanceData(MeshName mesh_name) {
	return m_instance_map[mesh_name];
}

std::shared_ptr<StructuredBuffer> MeshManager::GetInstanceBuffer(MeshName mesh_name) {
	return m_instance_buffer_map[mesh_name];
}

std::shared_ptr<ShaderResourceView> MeshManager::GetInstanceBufferView(MeshName mesh_name) {
	return m_instance_buffer_view_map[mesh_name];
}

int MeshManager::GetInstanceCount(MeshName mesh_name) {
	return m_instance_map.count(mesh_name);
}

MeshManager::InstanceIndexMap& MeshManager::GetInstanceIndexMap() {
	return m_instance_index_map;
}

std::vector<MeshManager::InstanceIndexData>& MeshManager::GetInstanceIndexData(MeshName mesh_name) {
	return m_instance_index_map[mesh_name];
}

int MeshManager::GetInstanceIndexCount(MeshName mesh_name) {
	return m_instance_index_map.count(mesh_name);
}

void MeshManager::ManageInstanceBufferMap(MeshName mesh_name) {
	if (!m_instance_map.count(mesh_name)) {
		if (m_instance_buffer_map.count(mesh_name)) {
			m_instance_buffer_map.erase(mesh_name);
		}
		else return;
	}

	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<D3DRenderer12> renderer = std::dynamic_pointer_cast<D3DRenderer12>(engine->GetRenderer());
	std::shared_ptr<Device> device = renderer->GetDevice();
	CommandQueue& command_queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	std::shared_ptr<CommandList> command_list = command_queue.GetCommandList();

	const auto& instance_data = m_instance_map[mesh_name];
	std::shared_ptr<StructuredBuffer> instance_buffer = command_list->CopyStructuredBuffer(instance_data, D3D12_RESOURCE_FLAG_NONE);
	m_instance_buffer_map[mesh_name] = instance_buffer;

	CommandQueue::FenceValueType fence_value = command_queue.ExecuteCommandList(command_list);
	command_queue.WaitForFenceValue(fence_value);

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0u;
	srv_desc.Buffer.NumElements = instance_data.size();
	srv_desc.Buffer.StructureByteStride = sizeof(InstanceData);
	std::shared_ptr<ShaderResourceView> instance_buffer_view = device->CreateShaderResourceView(instance_buffer, &srv_desc);
	m_instance_buffer_view_map[mesh_name] = instance_buffer_view;
}
