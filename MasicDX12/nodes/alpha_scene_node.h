#pragma once

#include <memory>

#include <DirectXMath.h>

#include "scene_node.h"

struct AlphaSceneNode;
typedef std::list<AlphaSceneNode*> AlphaSceneNodes;

struct AlphaSceneNode {
	std::shared_ptr<SceneNode> m_pNode;
	DirectX::XMFLOAT4X4 m_concat;
	float m_screenZ;

	bool const operator <(AlphaSceneNode const& other) { return m_screenZ < other.m_screenZ; }
};