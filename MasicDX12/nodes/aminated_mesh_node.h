#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "../graphics/mesh.h"

class AnimatedMeshNode : public SceneNode {
public:
	using MeshList = std::vector<std::shared_ptr<Mesh>>;
	using FinalTransformList = std::vector<DirectX::XMFLOAT4X4>;

	AnimatedMeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform);
	AnimatedMeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform, const MeshList& meshes);
	AnimatedMeshNode(const std::string& name, DirectX::FXMMATRIX transform);
	AnimatedMeshNode(const std::string& name, DirectX::FXMMATRIX transform, const MeshList& meshes);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	bool AddMesh(std::shared_ptr<Mesh> mesh);
	void RemoveMesh(std::shared_ptr<Mesh> mesh);
	const MeshList& GetMeshes();
	std::shared_ptr<Mesh> GetMesh(size_t index = 0);

	bool GetIsInstanced() const;
	void SetIsInstanced(bool is_instanced);

	FinalTransformList& GetFinalTransformList();
	const DirectX::XMFLOAT4X4& GetFinalTransform(size_t index = 0);
	void SetFinalTransformList(std::shared_ptr<FinalTransformList> final_transform_list);

protected:
	void CalcAABB();

	MeshList m_meshes;
	std::shared_ptr<FinalTransformList> m_final_transforms;
	bool m_instanced;
};