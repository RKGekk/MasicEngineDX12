#pragma once

#include <DirectXMath.h>

#include "light_type.h"

struct LightProperties {
	LightType m_LightType;
	bool m_is_lantern;
	DirectX::XMFLOAT3 m_Strength;
	float m_Attenuation[3];
	float m_Range;
	float m_Spot;
};