#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "../graphics/mesh.h"

class QualifierNode : public SceneNode {
public:
	QualifierNode(const std::string& name);

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid) override;
	virtual bool VRemoveChild(std::shared_ptr<SceneNode> cid) override;

	std::shared_ptr<SceneNode> GetNodesGroup(uint32_t group_id);

protected:
	using GroupID = uint32_t;
	using ChildIndex = uint32_t;
	std::unordered_map<GroupID, ChildIndex> m_children_map;
};