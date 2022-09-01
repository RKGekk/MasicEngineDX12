#pragma once

#include <memory>

class SceneNode;

class IVisitor {
public:
	IVisitor() = default;
	virtual ~IVisitor() = default;

	virtual void Visit(std::shared_ptr<SceneNode> scene_node) = 0;
};