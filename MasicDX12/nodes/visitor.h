#pragma once

class Scene;
class SceneNode;
class Mesh;

class Visitor {
public:
	Visitor() = default;
	virtual ~Visitor() = default;

	virtual void Visit(SceneNode& scene_node) = 0;
};