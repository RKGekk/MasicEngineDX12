#include "light_node.h"

LightNode::LightNode(const std::string& name, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t), m_light_props(props) {}

LightNode::LightNode(const std::string& name, const LightProperties& props, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true), m_light_props(props) {
	m_light_props = props;
}

HRESULT LightNode::VOnUpdate() {
	//uint32_t dirty_flags = Get().GetDirtyFlags();
	//uint32_t light_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Light);
	//uint32_t transform_flag = to_underlying(SceneNodeProperties::DirtyFlags::DF_Transform);
	//if ((dirty_flags & light_flag) || (dirty_flags & transform_flag)) {
	//	
	//}

	return SceneNode::VOnUpdate();
}

const LightProperties& LightNode::VGetLight() const {
	return m_light_props;
}

void LightNode::SetStrength(DirectX::XMFLOAT3 strength) {
	m_light_props.m_strength = strength;
}

void LightNode::SetAttenuation(float constant_attenuation, float linear_attenuation, float quadratic_attenuation) {
	m_light_props.m_attenuation[0] = constant_attenuation;
	m_light_props.m_attenuation[1] = linear_attenuation;
	m_light_props.m_attenuation[2] = quadratic_attenuation;
}

void LightNode::SetRange(float range) {
	m_light_props.m_range = range;
}

void LightNode::SetSpot(float spot) {
	m_light_props.m_spot = spot;
}

void LightNode::SetAmbient(DirectX::XMFLOAT3 ambient) {
	m_light_props.m_ambient = ambient;
}

SpotLight LightNode::GetSpotLight(DirectX::FXMMATRIX view) {
	SpotLight res = SpotLight();
	
	DirectX::XMVECTOR positionWS_xm = Get().CumulativePosition();
	DirectX::XMStoreFloat4(&res.PositionWS, positionWS_xm);
	DirectX::XMStoreFloat4(&res.PositionVS, DirectX::XMVector4Transform(positionWS_xm, view));

	DirectX::XMVECTOR directionWS_xm = Get().CumulativeDirection();
	DirectX::XMStoreFloat4(&res.DirectionWS, directionWS_xm);
	DirectX::XMStoreFloat4(&res.DirectionVS, DirectX::XMVector4Transform(directionWS_xm, view));

	res.Color = DirectX::XMFLOAT4(m_light_props.m_strength.x, m_light_props.m_strength.y, m_light_props.m_strength.z, 1.0f);
	res.Ambient = m_light_props.m_ambient;
	res.SpotAngle = m_light_props.m_spot;
	res.ConstantAttenuation = m_light_props.m_attenuation[0];
	res.LinearAttenuation = m_light_props.m_attenuation[1];
	res.QuadraticAttenuation = m_light_props.m_attenuation[2];

	return res;
}

PointLight LightNode::GetPointLight(DirectX::FXMMATRIX view) {
	PointLight res = PointLight();

	DirectX::XMVECTOR positionWS_xm = Get().CumulativePosition();
	DirectX::XMStoreFloat4(&res.PositionWS, positionWS_xm);
	DirectX::XMStoreFloat4(&res.PositionVS, DirectX::XMVector4Transform(positionWS_xm, view));

	res.Color = DirectX::XMFLOAT4(m_light_props.m_strength.x, m_light_props.m_strength.y, m_light_props.m_strength.z, 1.0f);
	res.Ambient = m_light_props.m_ambient;
	res.ConstantAttenuation = m_light_props.m_attenuation[0];
	res.LinearAttenuation = m_light_props.m_attenuation[1];
	res.QuadraticAttenuation = m_light_props.m_attenuation[2];

	return res;
}

DirectionalLight LightNode::GetDirectionalLight(DirectX::FXMMATRIX view) {
	DirectionalLight res = DirectionalLight();

	DirectX::XMVECTOR directionWS_xm = Get().CumulativeDirection();
	DirectX::XMStoreFloat4(&res.DirectionWS, directionWS_xm);
	DirectX::XMStoreFloat4(&res.DirectionVS, DirectX::XMVector4Transform(directionWS_xm, view));

	res.Color = DirectX::XMFLOAT4(m_light_props.m_strength.x, m_light_props.m_strength.y, m_light_props.m_strength.z, 1.0f);
	res.Ambient = m_light_props.m_ambient;

	return res;
}