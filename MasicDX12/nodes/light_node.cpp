#include "light_node.h"

LightNode::LightNode(const std::string& name, const LightProperties& props, const DirectX::XMFLOAT4X4* t) : SceneNode(name, t), m_light_props(props) {}

LightNode::LightNode(const std::string& name, const LightProperties& props, DirectX::FXMMATRIX to) : SceneNode(name, to, DirectX::XMMatrixIdentity(), true), m_light_props(props) {
	m_light_props = props;
}

const LightProperties& LightNode::VGetLight() const {
	return m_light_props;
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
	res.Ambient = 0.0f;
	res.SpotAngle = m_light_props.m_spot;
	res.ConstantAttenuation = m_light_props.m_attenuation[0];
	res.LinearAttenuation = m_light_props.m_attenuation[1];
	res.QuadraticAttenuation = m_light_props.m_attenuation[3];

	return res;
}

PointLight LightNode::GetPointLight() {
	PointLight res = PointLight();

	DirectX::XMVECTOR positionWS_xm = Get().CumulativePosition();
	DirectX::XMStoreFloat4(&res.PositionWS, positionWS_xm);
	DirectX::XMStoreFloat4(&res.PositionVS, DirectX::XMVector4Transform(positionWS_xm, view));

	res.Color = DirectX::XMFLOAT4(m_light_props.m_strength.x, m_light_props.m_strength.y, m_light_props.m_strength.z, 1.0f);
	res.Ambient = 0.0f;
	res.ConstantAttenuation = m_light_props.m_attenuation[0];
	res.LinearAttenuation = m_light_props.m_attenuation[1];
	res.QuadraticAttenuation = m_light_props.m_attenuation[3];

	return res;
}

DirectionalLight LightNode::GetDirectionalLight() {
	DirectionalLight res = DirectionalLight();

	DirectX::XMVECTOR directionWS_xm = Get().CumulativeDirection();
	DirectX::XMStoreFloat4(&res.DirectionWS, directionWS_xm);
	DirectX::XMStoreFloat4(&res.DirectionVS, DirectX::XMVector4Transform(directionWS_xm, view));

	res.Color = DirectX::XMFLOAT4(m_light_props.m_strength.x, m_light_props.m_strength.y, m_light_props.m_strength.z, 1.0f);
	res.Ambient = 0.0f;

	return res;
}