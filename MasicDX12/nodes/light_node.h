#pragma once

#include <memory>

#include <DirectXMath.h>

#include "scene_node.h"
#include "../actors/base_render_component.h"
#include "light_properties.h"
#include "../graphics/spot_light.h"
#include "../graphics/directional_light.h"
#include "../graphics/point_light.h"

class LightNode;
typedef std::list<std::shared_ptr<LightNode>> Lights;

class LightNode : public SceneNode {
protected:
	LightProperties m_light_props;

public:
	LightNode(const std::string& name, const LightProperties& props, const DirectX::XMFLOAT4X4* t);
	LightNode(const std::string& name, const LightProperties& props, DirectX::FXMMATRIX to);

	LightNode(const std::string& name, const SpotLight& spot_light, const DirectX::XMFLOAT4X4* t);
	LightNode(const std::string& name, const SpotLight& spot_light, DirectX::FXMMATRIX to);

	LightNode(const std::string& name, const PointLight& point_light, const DirectX::XMFLOAT4X4* t);
	LightNode(const std::string& name, const PointLight& point_light, DirectX::FXMMATRIX to);

	LightNode(const std::string& name, const DirectionalLight& directional_light, const DirectX::XMFLOAT4X4* t);
	LightNode(const std::string& name, const DirectionalLight& directional_light, DirectX::FXMMATRIX to);

	const LightProperties& VGetLight() const;

	SpotLight GetSpotLight(DirectX::FXMMATRIX view);
	PointLight GetPointLight();
	DirectionalLight GetDirectionalLight();

private:
	void SetLightProps(const SpotLight& spot_light);
	void SetLightProps(const PointLight& point_light);
	void SetLightProps(const DirectionalLight& directional_light);
};