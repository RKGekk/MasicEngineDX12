#pragma once

#include <vector>

#include <DirectXMath.h>

#include "rigid_body.h"
#include "force_generator.h"

class ForceRegistry {
protected:
    struct ForceRegistration {
        RigidBody* body;
        ForceGenerator* fg;
    };

    typedef std::vector<ForceRegistration> Registry;
    Registry registrations;

public:
    void add(RigidBody* body, ForceGenerator* fg);
    void remove(RigidBody* body, ForceGenerator* fg);
    void clear();
    void updateForces(float duration);
};