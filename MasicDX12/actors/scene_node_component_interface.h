#pragma once

#include <memory>

#include "actor_component.h"

class SceneNode;

class SceneNodeComponentInterface : public ActorComponent {
public:
	virtual std::shared_ptr<SceneNode> VGetSceneNode() = 0;
};