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

class LightNode : public SceneNode {
public:
	LightNode(const std::string& name, const LightProperties& props, const DirectX::XMFLOAT4X4* t);
	LightNode(const std::string& name, const LightProperties& props, DirectX::FXMMATRIX to);

	virtual HRESULT VOnUpdate() override;

	const LightProperties& VGetLight() const;

	void SetStrength(DirectX::XMFLOAT3 strength);
	void SetAttenuation(float constant_attenuation, float linear_attenuation, float quadratic_attenuation);
	void SetRange(float range);
	void SetSpot(float spot);
	void SetAmbient(DirectX::XMFLOAT3 ambient);

	SpotLight GetSpotLight(DirectX::FXMMATRIX view);
	PointLight GetPointLight(DirectX::FXMMATRIX view);
	DirectionalLight GetDirectionalLight(DirectX::FXMMATRIX view);

protected:
	LightProperties m_light_props;
};