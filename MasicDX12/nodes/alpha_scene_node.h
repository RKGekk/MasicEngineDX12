#pragma once

#include <memory>

#include <DirectXMath.h>

#include "i_scene_node.h"

struct AlphaSceneNode;
typedef std::list<AlphaSceneNode*> AlphaSceneNodes;

struct AlphaSceneNode {
	std::shared_ptr<ISceneNode> m_pNode;
	DirectX::XMFLOAT4X4 m_Concat;
	float m_ScreenZ;

	bool const operator <(AlphaSceneNode const& other) { return m_ScreenZ < other.m_ScreenZ; }
};