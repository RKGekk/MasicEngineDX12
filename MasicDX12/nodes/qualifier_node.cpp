#include "qualifier_node.h"

QualifierNode::QualifierNode(const std::string& name) : SceneNode(name) {
	using namespace std::literals;
	m_children_map[0u] = m_children.size();
	std::shared_ptr<SceneNode> GroupNode = std::make_shared<SceneNode>("GroupID0"s, 0u);
	m_children.push_back(GroupNode);
}

bool QualifierNode::VAddChild(std::shared_ptr<SceneNode> kid) {
	using namespace std::literals;
	uint32_t group_id = kid->Get().GetGroupID();
	if (!m_children_map.count(group_id)) {
		m_children_map[group_id] = m_children.size();
		std::shared_ptr<SceneNode> GroupNode = std::make_shared<SceneNode>("GroupID"s + std::to_string(group_id), group_id);
		m_children.push_back(GroupNode);
	}
	uint32_t index = m_children_map[group_id];
	return m_children[index]->VAddChild(kid);
}

bool QualifierNode::VRemoveChild(std::shared_ptr<SceneNode> cid) {
	bool anythingRemoved = false;
	size_t sz = m_children.size();
	for (uint32_t i = 0u; i < sz; ++i) {
		if (m_children[i]->VRemoveChild(cid)) {
			anythingRemoved = true;
		}
	}
	return anythingRemoved;
}

std::shared_ptr<SceneNode> QualifierNode::GetNodesGroup(uint32_t group_id) {
	if (m_children_map.count(group_id)) {
		uint32_t index = m_children_map[group_id];
		return m_children[index];
	}
	return std::shared_ptr<SceneNode>();
}
