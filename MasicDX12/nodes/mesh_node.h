#pragma once

#include <memory>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "../graphics/mesh.h"

class MeshNode : public SceneNode {
public:
	using MeshList = std::vector<std::shared_ptr<Mesh>>;

	MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform);
	MeshNode(const std::string& name, const DirectX::XMFLOAT4X4& transform, const MeshList& meshes);
	MeshNode(const std::string& name, DirectX::FXMMATRIX transform);
	MeshNode(const std::string& name, DirectX::FXMMATRIX transform, const MeshList& meshes);

	virtual HRESULT VOnRestore() override;
	virtual HRESULT VOnUpdate() override;

	bool AddMesh(std::shared_ptr<Mesh> mesh);
	void RemoveMesh(std::shared_ptr<Mesh> mesh);
	const MeshList& GetMeshes();
	std::shared_ptr<Mesh> GetMesh(size_t index = 0);

	bool GetIsInstanced() const;
	void SetIsInstanced(bool is_instanced);

protected:
	void CalcAABB();

	using MeshList = std::vector<std::shared_ptr<Mesh>>;

	MeshList m_meshes;
	bool m_instanced;
};