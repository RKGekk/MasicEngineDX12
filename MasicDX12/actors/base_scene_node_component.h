#pragma once

#include <memory>
#include <string>

#include "actor_component.h"

class SceneNode;
class CameraNode;

class BaseSceneNodeComponent : public ActorComponent {
public:
	BaseSceneNodeComponent();
	virtual ~BaseSceneNodeComponent();

	virtual std::shared_ptr<SceneNode> VGetSceneNode();
	
	virtual bool VInit(const pugi::xml_node& data) override;
	virtual void VPostInit() override;
	virtual void VUpdate(const GameTimerDelta& delta) override;
	virtual void VOnChanged() override;

protected:
	virtual void VRegisterEvents();

	virtual bool VDelegateInit(const pugi::xml_node& data) = 0;
	virtual void VDelegatePostInit() = 0;
	virtual void VDelegateUpdate(const GameTimerDelta& delta) = 0;

	bool m_current_gen_updated;

private:
	std::shared_ptr<SceneNode> m_scene_node;
	uint32_t m_generation;
};