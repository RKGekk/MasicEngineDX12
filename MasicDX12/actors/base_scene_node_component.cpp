#include "base_scene_node_component.h"

#include "../tools/string_utility.h"
#include "../nodes/scene_node.h"
#include "transform_component.h"
#include "../events/evt_data_new_scene_component.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/i_event_manager.h"

BaseSceneNodeComponent::BaseSceneNodeComponent() : m_generation(0u) {}

BaseSceneNodeComponent::~BaseSceneNodeComponent() {
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	std::shared_ptr<EvtData_Destroy_Scene_Component> pNewActorEvent = std::make_shared<EvtData_Destroy_Scene_Component>(act->GetId(), VGetId(), scene_node);
	IEventManager::Get()->VQueueEvent(pNewActorEvent);
}

std::shared_ptr<SceneNode> BaseSceneNodeComponent::VGetSceneNode() {
	return m_scene_node;
}

bool BaseSceneNodeComponent::VInit(const pugi::xml_node& data) {
	using namespace std::literals;
	VRegisterEvents();
	m_scene_node = std::make_shared<SceneNode>("NoName"s, DirectX::XMMatrixIdentity());
	return VDelegateInit(data);
}

void BaseSceneNodeComponent::VPostInit() {
	std::shared_ptr<Actor> act = GetOwner();
	std::string name = act->GetName();
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	scene_node->SetName(name);
	std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
	if (tc) {
		scene_node->SetTransform(tc->GetTransform());
	}
	VDelegatePostInit();
	std::shared_ptr<EvtData_New_Scene_Component> pNewSceneNodeEvent = std::make_shared<EvtData_New_Scene_Component>(act->GetId(), VGetId(), scene_node);
	IEventManager::Get()->VQueueEvent(pNewSceneNodeEvent);
}

void BaseSceneNodeComponent::VUpdate(const GameTimerDelta& delta) {
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
	if (tc && (tc->GetGeneration() > m_generation)) {
		std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
		scene_node->SetTransform(tc->GetTransform());
	}
	VDelegateUpdate(delta);
}

void BaseSceneNodeComponent::VOnChanged() {}

void BaseSceneNodeComponent::VRegisterEvents() {
	if (!m_events_registered) {
		REGISTER_EVENT(EvtData_New_Scene_Component);
		m_events_registered = true;
	}
}
