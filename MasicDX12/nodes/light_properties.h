#pragma once

#include <DirectXMath.h>

#include "light_type.h"

struct LightProperties {
	LightType m_light_type;
	DirectX::XMFLOAT3 m_strength;
	float m_attenuation[3]; // Constant, LinearAttenuation, Quadratic
	float m_range;
	float m_spot;
	DirectX::XMFLOAT3 m_ambient;
};