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

	bool AddMesh(std::shared_ptr<Mesh> mesh);
	void RemoveMesh(std::shared_ptr<Mesh> mesh);
	const MeshList& GetMeshes();
	std::shared_ptr<Mesh> GetMesh(size_t index = 0);

	const DirectX::BoundingBox& GetAABB() const;
	const DirectX::BoundingSphere& GetSphere() const;

protected:
	using MeshList = std::vector<std::shared_ptr<Mesh>>;

	MeshList m_meshes;
	DirectX::BoundingBox m_AABB_merged;
	DirectX::BoundingSphere m_sphere_merged;
};