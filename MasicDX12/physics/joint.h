#pragma once

#include <DirectXMath.h>

#include "contact_generator.h"

class Joint : public ContactGenerator {
public:
    RigidBody* body[2];
    DirectX::XMFLOAT3 position[2];
    float error;

    void set(RigidBody* a, const DirectX::XMFLOAT3& a_pos, RigidBody* b, const DirectX::XMFLOAT3& b_pos, float error);
    void set(RigidBody* a, DirectX::XMVECTOR a_pos, RigidBody* b, DirectX::XMVECTOR b_pos, float error);
    unsigned addContact(Contact* contact, unsigned limit) const override;
};