#include "light_node.h"

LightNode::LightNode(const std::string& name, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t), m_light_props(props) {}

LightNode::LightNode(const std::string& name, const LightProperties& props, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true), m_light_props(props) {
	m_light_props = props;
}

LightNode::LightNode(const std::string& name, const SpotLight& spot_light, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t) {
	SetLightProps(spot_light);
}

LightNode::LightNode(const std::string& name, const SpotLight& spot_light, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true) {
	SetLightProps(spot_light);
}

LightNode::LightNode(const std::string& name, const PointLight& point_light, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t) {
	SetLightProps(point_light);
}

LightNode::LightNode(const std::string& name, const PointLight& point_light, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true) {
	SetLightProps(point_light);
}

LightNode::LightNode(const std::string& name, const DirectionalLight& directional_light, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t) {
	SetLightProps(directional_light);
}

LightNode::LightNode(const std::string& name, const DirectionalLight& directional_light, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true) {
	SetLightProps(directional_light);
}

const LightProperties& LightNode::VGetLight() const {
	return m_light_props;
}

SpotLight LightNode::GetSpotLight() {
	SpotLight res();
	return SpotLight();
}

void LightNode::SetLightProps(const SpotLight& spot_light) {
	m_light_props.m_light_type = LightType::SPOT;
	m_light_props.m_strength = DirectX::XMFLOAT3(spot_light.Color.x, spot_light.Color.y, spot_light.Color.z);
	m_light_props.m_attenuation[0] = spot_light.ConstantAttenuation;
	m_light_props.m_attenuation[1] = spot_light.LinearAttenuation;
	m_light_props.m_attenuation[2] = spot_light.QuadraticAttenuation;
	m_light_props.m_range = 0.0f;
	m_light_props.m_spot = spot_light.SpotAngle;
}

void LightNode::SetLightProps(const PointLight& point_light) {
	m_light_props.m_light_type = LightType::POINT;
	m_light_props.m_strength = DirectX::XMFLOAT3(point_light.Color.x, point_light.Color.y, point_light.Color.z);
	m_light_props.m_attenuation[0] = point_light.ConstantAttenuation;
	m_light_props.m_attenuation[1] = point_light.LinearAttenuation;
	m_light_props.m_attenuation[2] = point_light.QuadraticAttenuation;
	m_light_props.m_range = 0.0f;
	m_light_props.m_spot = 0.0f;
}

void LightNode::SetLightProps(const DirectionalLight& directional_light) {
	m_light_props.m_light_type = LightType::DIRECTIONAL;
	m_light_props.m_strength = DirectX::XMFLOAT3(directional_light.Color.x, directional_light.Color.y, directional_light.Color.z);
	m_light_props.m_attenuation[0] = 0.0f;
	m_light_props.m_attenuation[1] = 0.0f;
	m_light_props.m_attenuation[2] = 0.0f;
	m_light_props.m_range = 0.0f;
	m_light_props.m_spot = 0.0f;
}