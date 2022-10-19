#include "light_component.h"

#include "../tools/string_utility.h"
#include "../nodes/light_node.h"
#include "../application.h"
#include "transform_component.h"
#include "../events/evt_data_destroy_scene_component.h"
#include "../events/i_event_manager.h"

const std::string LightComponent::g_Name = "LightComponent";

LightComponent::LightComponent() {}

LightComponent::LightComponent(const pugi::xml_node& data) {
	VDelegateInit(data);
}

LightComponent::~LightComponent() {
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	std::shared_ptr<EvtData_Destroy_Scene_Component> pDestroyActorComponentEvent = std::make_shared<EvtData_Destroy_Scene_Component>(act->GetId(), VGetId(), scene_node);
	IEventManager::Get()->VQueueEvent(pDestroyActorComponentEvent);
}

bool LightComponent::VDelegateInit(const pugi::xml_node& data) {
	using namespace std::literals;

	pugi::xml_node light_node = data.child("Light");
	if (!light_node) return false;

	LightProperties props = {};

	std::string light_type_string = light_node.attribute("type").as_string();
	props.m_light_type = GetLightType(light_type_string);

	bool is_latern = light_node.attribute("lantern").as_bool();

	DirectX::XMFLOAT3 default_strength = { 1.0f, 1.0f, 1.0f };
	props.m_strength = colorfromattr3f(light_node.child("Strength"), default_strength);

	pugi::xml_node attenuation_node = light_node.child("Attenuation");
	props.m_attenuation[0] = attenuation_node.attribute("const").as_float();
	props.m_attenuation[1] = attenuation_node.attribute("linear").as_float();
	props.m_attenuation[2] = attenuation_node.attribute("exp").as_float();

	pugi::xml_node shape_node = light_node.child("Shape");
	props.m_range = shape_node.attribute("range").as_float();
	props.m_spot = shape_node.attribute("spot").as_float();

	DirectX::XMFLOAT3 default_ambient = { 0.01f, 0.01f, 0.01f };
	props.m_ambient = colorfromattr3f(light_node.child("Ambient"), default_ambient);

	m_loaded_scene_node = std::make_shared<LightNode>("LightNode"s, props, DirectX::XMMatrixIdentity());

	return true;
}

void LightComponent::VDelegatePostInit() {
	std::shared_ptr<SceneNode> scene_node = VGetSceneNode();
	scene_node->VAddChild(m_loaded_scene_node);
}

void LightComponent::VDelegateUpdate(const GameTimerDelta& delta) {
	if (m_current_gen_updated) {
		m_loaded_scene_node->AddDirtyFlags(to_underlying(SceneNodeProperties::DirtyFlags::DF_Light));
	}
}

const std::string& LightComponent::VGetName() const {
	return LightComponent::g_Name;
}

pugi::xml_node LightComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<LightNode> LightComponent::VGetLightNode() {
	return m_loaded_scene_node;
}

LightType LightComponent::GetLightType(const std::string& light_type_string) {
	if (light_type_string == "PointLight") return LightType::POINT;
	if (light_type_string == "SpotLight") return LightType::SPOT;
	return LightType::DIRECTIONAL;
}