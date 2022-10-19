#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "../graphics/directx12_wrappers/command_list.h"

class MeshNode;
class SceneNode;
class BasicCameraNode;

class MeshManager {
	friend class Scene;
public:
	struct InstanceData {
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4X4 InverseTransposeWorld;
		DirectX::XMFLOAT4X4 TexureUVTransform;
	};

	struct InstanceIndexData {
		uint32_t InstanceIndex;
		uint32_t Pad1;
		uint32_t Pad2;
		uint32_t Pad3;
	};

	using MeshName = std::string;
	using MeshList = std::vector<std::shared_ptr<MeshNode>>;
	using MeshMap = std::unordered_map<MeshName, MeshList>;

	using InstanceMap = std::unordered_map<MeshName, std::vector<InstanceData>>;
	using InstanceBufferMap = std::unordered_map<MeshName, std::shared_ptr<StructuredBuffer>>;
	using InstanceBufferViewMap = std::unordered_map<MeshName, std::shared_ptr<ShaderResourceView>>;
	using InstanceIndexMap = std::unordered_map<MeshName, std::vector<InstanceIndexData>>;

	MeshManager();

	void AddMesh(std::shared_ptr<SceneNode> node);
	void RemoveMesh(std::shared_ptr<SceneNode> node);

	int GetMeshCount(std::shared_ptr<SceneNode> node);
	int GetMeshCount(MeshName mesh_name);
	const MeshList& GetMeshList(MeshName mesh_name) const;
	const MeshMap& GetMeshMap() const;

	void CalcInstances(const BasicCameraNode& camera);
	void UpdateInstancesBuffer();

	InstanceMap& GetInstanceMap();
	std::vector<InstanceData>& GetInstanceData(MeshName mesh_name);
	std::shared_ptr<StructuredBuffer> GetInstanceBuffer(MeshName mesh_name);
	std::shared_ptr<ShaderResourceView> GetInstanceBufferView(MeshName mesh_name);
	int GetInstanceCount(MeshName mesh_name);
	InstanceIndexMap& GetInstanceIndexMap();
	std::vector<InstanceIndexData>& GetInstanceIndexData(MeshName mesh_name);
	int GetInstanceIndexCount(MeshName mesh_name);

protected:
	void ManageInstanceBufferMap(MeshName mesh_name);

	MeshMap m_mesh_map;
	InstanceMap m_instance_map;
	InstanceIndexMap m_instance_index_map;

	InstanceBufferMap m_instance_buffer_map;
	InstanceBufferViewMap m_instance_buffer_view_map;
};