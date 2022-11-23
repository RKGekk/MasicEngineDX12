#pragma once

#include <string>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "../actors/actor.h"
#include "../graphics/material.h"
#include "../tools/memory_utility.h"
#include "alpha_type.h"

class SceneNodeProperties {
	friend class SceneNode;

protected:
	std::string m_name;
	DirectX::XMFLOAT4X4 m_to_parent;
	DirectX::XMFLOAT4X4 m_from_parent;
	DirectX::XMFLOAT4X4 m_to_root;
	DirectX::XMFLOAT4X4 m_from_root;

	uint32_t m_dirty_flags;
	uint32_t m_generation;
	uint32_t m_group_id;

	DirectX::BoundingBox m_AABB;
	DirectX::BoundingSphere m_sphere;

	DirectX::BoundingBox m_AABB_cumulative;
	DirectX::BoundingSphere m_sphere_cumulative;

	DirectX::BoundingBox m_AABB_merged;
	DirectX::BoundingSphere m_sphere_merged;
	
	bool m_active;

public:
	enum class DirtyFlags {
		DF_None      = 0,
		DF_Light     = (1 << 0),
		DF_Mesh      = (1 << 1),
		DF_Transform = (1 << 2),
		DF_Camera    = (1 << 3),
		DF_All = DF_Light | DF_Mesh | DF_Transform | DF_Camera
	};

	SceneNodeProperties();

	DirectX::XMMATRIX ToParent() const;
	DirectX::XMMATRIX ToParentT() const;
	const DirectX::XMFLOAT4X4& ToParent4x4() const;
	DirectX::XMFLOAT4X4 ToParent4x4T() const;

	DirectX::XMMATRIX ToRoot() const;
	DirectX::XMMATRIX ToRootT() const;
	const DirectX::XMFLOAT4X4& ToRoot4x4() const;
	DirectX::XMFLOAT4X4 ToRoot4x4T() const;

	DirectX::XMVECTOR ToParentTranslation() const;
	DirectX::XMFLOAT4 ToParentTranslation4() const;
	DirectX::XMFLOAT3 ToParentTranslation3() const;

	DirectX::XMVECTOR ToRootTranslation() const;
	DirectX::XMFLOAT4 ToRootTranslation4() const;
	DirectX::XMFLOAT3 ToRootTranslation3() const;

	DirectX::XMVECTOR ToParentDirection() const;
	DirectX::XMFLOAT3 ToParentDirection3f() const;
	DirectX::XMVECTOR ToParentUp() const;
	DirectX::XMFLOAT3 ToParentUp3f() const;

	DirectX::XMVECTOR ToRootDirection() const;
	DirectX::XMFLOAT3 ToRootDirection3f() const;
	DirectX::XMVECTOR ToRootUp() const;
	DirectX::XMFLOAT3 ToRootUp3f() const;

	DirectX::XMMATRIX FromParent() const;
	const DirectX::XMFLOAT4X4& FromParent4x4() const;
	DirectX::XMFLOAT4X4 FromParent4x4T() const;

	DirectX::XMMATRIX FromRoot() const;
	const DirectX::XMFLOAT4X4& FromRoot4x4() const;
	DirectX::XMFLOAT4X4 FromRoot4x4T() const;

	const char* NameCstr() const;
	const std::string& Name() const;

	uint32_t GetDirtyFlags() const;
	uint32_t GetGroupID() const;
	uint32_t GetGeneration() const;

	const DirectX::BoundingBox& AABB() const;
	const DirectX::BoundingSphere& Sphere() const;

	const DirectX::BoundingBox& CumulativeAABB() const;
	const DirectX::BoundingSphere& CumulativeSphere() const;

	const DirectX::BoundingBox& MergedAABB() const;
	const DirectX::BoundingSphere& MergedSphere() const;
};