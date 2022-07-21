#pragma once

#include "rigid_body.h"

class ForceGenerator {
public:
	virtual void updateForce(RigidBody *body, float duration) = 0;
};