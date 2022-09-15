#include "light_component.h"

#include "../tools/string_utility.h"
#include "../nodes/light_node.h"
#include "../application.h"
#include "transform_component.h"

const std::string LightComponent::g_Name = "LightComponent";

LightComponent::LightComponent() {}

LightComponent::LightComponent(const pugi::xml_node& data) {
	Init(data);
}

bool LightComponent::VInit(const pugi::xml_node& data) {
	return Init(data);
}

void LightComponent::VPostInit() {
	std::shared_ptr<Actor> act = GetOwner();
	std::string name = act->GetName();
	m_scene_node->SetName(name);
	std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
	if (tc) {
		m_scene_node->SetTransform(tc->GetTransform());
	}
}

void LightComponent::VUpdate(const GameTimerDelta& delta) {
	std::shared_ptr<Actor> act = GetOwner();
	std::shared_ptr<TransformComponent> tc = act->GetComponent<TransformComponent>(ActorComponent::GetIdFromName("TransformComponent")).lock();
	if (tc) {
		m_scene_node->SetTransform(tc->GetTransform());
	}
}

const std::string& LightComponent::VGetName() const {
	return LightComponent::g_Name;
}

pugi::xml_node LightComponent::VGenerateXml() {
	return pugi::xml_node();
}

std::shared_ptr<SceneNode> LightComponent::VGetSceneNode() {
	return m_scene_node;
}

LightType LightComponent::GetLightType(const std::string& light_type_string) {
	if (light_type_string == "PointLight") return LightType::POINT;
	if (light_type_string == "SpotLight") return LightType::SPOT;
	return LightType::DIRECTIONAL;
}

bool LightComponent::Init(const pugi::xml_node& data) {
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

	m_scene_node = std::make_shared<LightNode>("NoName"s, props, DirectX::XMMatrixIdentity());

	return true;
}