#pragma once

class SceneNode;

class IVisitor {
public:
	IVisitor() = default;
	virtual ~IVisitor() = default;

	virtual void Visit(SceneNode& scene_node) = 0;
};