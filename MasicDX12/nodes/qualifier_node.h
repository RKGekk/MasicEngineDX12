#pragma once

#include <memory>
#include <unordered_map>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "scene_node.h"
#include "../graphics/mesh.h"

class QualifierNode : public SceneNode {
public:
	QualifierNode(const std::string& name);

	virtual bool VAddChild(std::shared_ptr<SceneNode> kid) override;
	virtual bool VRemoveChild(std::shared_ptr<SceneNode> cid) override;

protected:
	std::unordered_map<int, std::shared_ptr<SceneNode>> m_children_map;
};