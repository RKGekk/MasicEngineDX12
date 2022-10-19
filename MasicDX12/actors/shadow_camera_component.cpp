#include "shadow_camera_component.h"

#include "../tools/string_utility.h"
#include "../nodes/camera_node.h"
#include "../nodes/shadow_camera_node.h"
#include "../nodes/qualifier_node.h"
#include "../application.h"
#include "../engine/engine.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/i_event_manager.h"
#include "transform_component.h"
#include "light_component.h"

const std::string ShadowCameraComponent::g_Name = "ShadowCameraComponent";

ShadowCameraComponent::ShadowCameraComponent() : m_shadow_map_width(2048), m_shadow_map_height(2048), m_depth_bias(100000), m_depth_bias_clamp(0.1f), m_slope_scaled_depth_bias(1.0f) {}

ShadowCameraComponent::ShadowCameraComponent(const pugi::xml_node& data) : m_shadow_map_width(2048), m_shadow_map_height(2048), m_depth_bias(100000), m_depth_bias_clamp(0.1f), m_slope_scaled_depth_bias(1.0f) {
	VDelegateInit(data);
}

ShadowCameraComponent::~ShadowCameraComponent() {
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	std::shared_ptr<EvtData_Destroy_Scene_Component> pDestroyActorComponentEvent = std::make_shared<EvtData_Destroy_Scene_Component>(act->GetId(), VGetId(), scene_node);
	IEventManager::Get()->VQueueEvent(pDestroyActorComponentEvent);
}

void ShadowCameraComponent::VDelegatePostInit() {
	std::shared_ptr<Engine> engine = Engine::GetEngine();
	std::shared_ptr<BaseEngineLogic> engine_logic = engine->GetGameLogic();
	std::shared_ptr<HumanView> human_view = engine_logic->GetHumanView();
	std::shared_ptr<Scene> scene = human_view->VGetScene();
	std::shared_ptr<SceneNode> root_node = scene->GetRootNode()->GetNodesGroup(0u);

	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<LightComponent> lc = MakeStrongPtr(act->GetComponent<LightComponent>(ActorComponent::GetIdFromName("LightComponent")));
	std::shared_ptr<LightNode> light_node = lc->VGetLightNode();

	ShadowCameraNode::ShadowCameraProps shadow_props = {};
	shadow_props.ShadowMapWidth = m_shadow_map_width;
	shadow_props.ShadowMapHeight = m_shadow_map_height;
	shadow_props.DepthBias = m_depth_bias;
	shadow_props.DepthBiasClamp = m_depth_bias_clamp;
	shadow_props.SlopeScaledDepthBias = m_slope_scaled_depth_bias;

	m_loaded_scene_node = std::make_shared<ShadowCameraNode>("ShadowCameraComponent"s, DirectX::XMMatrixIdentity(), root_node, light_node, shadow_props);

	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	scene_node->VAddChild(m_loaded_scene_node);
}

void ShadowCameraComponent::VDelegateUpdate(const GameTimerDelta& delta) {
	if (m_current_gen_updated) {
		m_loaded_scene_node->AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Camera));
	}
}

const std::string& ShadowCameraComponent::VGetName() const {
	return ShadowCameraComponent::g_Name;
}

pugi::xml_node ShadowCameraComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<ShadowCameraNode> ShadowCameraComponent::VGetCameraNode() {
	return m_loaded_scene_node;
}

int ShadowCameraComponent::GetSMapWidth() {
	return m_shadow_map_width;
}

int ShadowCameraComponent::GetSMapHeight() {
	return m_shadow_map_height;
}

int ShadowCameraComponent::GetDepthBias() {
	return m_depth_bias;
}

float ShadowCameraComponent::GetDepthBiasClamp() {
	return m_depth_bias_clamp;
}

float ShadowCameraComponent::GetSlopeScaledDepthBias() {
	return m_slope_scaled_depth_bias;
}


bool ShadowCameraComponent::VDelegateInit(const pugi::xml_node& data) {
	using namespace std::literals;

	m_shadow_map_width = data.child("ShadowMapSize").text().as_int(m_shadow_map_width);
	m_shadow_map_height = data.child("ShadowMapSize").text().as_int(m_shadow_map_height);
	m_depth_bias = data.child("DepthBias").text().as_int(m_depth_bias);
	m_depth_bias_clamp = data.child("DepthBiasClamp").text().as_float(m_depth_bias_clamp);
	m_slope_scaled_depth_bias = data.child("SlopeScaledDepthBias").text().as_float(m_slope_scaled_depth_bias);

	return true;
}
