#pragma once

#include <DirectXMath.h>

#include "force_generator.h"

class Gravity : public ForceGenerator {
	DirectX::XMFLOAT3 m_gravity;

public:

	Gravity(const DirectX::XMFLOAT3 &gravity);
	Gravity(DirectX::FXMVECTOR gravity);

	virtual void updateForce(RigidBody *body, float duration) override;
};