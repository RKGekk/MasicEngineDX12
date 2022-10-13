#include "base_scene_node_component.h"

#include "../tools/string_utility.h"
#include "../nodes/scene_node.h"
#include "transform_component.h"
#include "../events/evt_data_new_scene_component.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/evt_data_modified_scene_component.h"
#include "../events/i_event_manager.h"

BaseSceneNodeComponent::BaseSceneNodeComponent() : m_generation(0u), m_current_gen_updated(false) {}

BaseSceneNodeComponent::~BaseSceneNodeComponent() {}

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
	m_current_gen_updated = false;
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
	uint32_t tc_gen = tc->GetGeneration();
	if (tc && (tc_gen > m_generation)) {
		std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
		scene_node->SetTransform(tc->GetTransform());
		scene_node->SetScale(tc->GetScale3f());
		m_generation = tc_gen;
		m_current_gen_updated = true;
		std::shared_ptr<EvtData_Modified_Scene_Component> pEvent(new EvtData_Modified_Scene_Component(m_pOwner->GetId(), VGetId(), scene_node));
		IEventManager::Get()->VTriggerEvent(pEvent);
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
