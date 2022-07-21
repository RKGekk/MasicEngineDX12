#include "joint.h"
#include <cmath>

void Joint::set(RigidBody* a, const DirectX::XMFLOAT3& a_pos, RigidBody* b, const DirectX::XMFLOAT3& b_pos, float error) {
    body[0] = a;
    body[1] = b;

    position[0] = a_pos;
    position[1] = b_pos;

    Joint::error = error;
}

void Joint::set(RigidBody* a, DirectX::XMVECTOR a_pos, RigidBody* b, DirectX::XMVECTOR b_pos, float error) {
    body[0] = a;
    body[1] = b;

    DirectX::XMStoreFloat3(&position[0], a_pos);
    DirectX::XMStoreFloat3(&position[1], b_pos);

    Joint::error = error;
}

unsigned Joint::addContact(Contact* contact, unsigned limit) const {
    using namespace DirectX;

    XMVECTOR a_pos_world = body[0]->getPointInWorldSpace(position[0]);
    XMVECTOR b_pos_world = body[1]->getPointInWorldSpace(position[1]);

    XMVECTOR a_to_b = b_pos_world - a_pos_world;
    XMVECTOR normal = a_to_b;
    normal = XMVector3Normalize(normal);
    float length = XMVectorGetX(XMVector3Length(a_to_b));

    if (fabsf(length) > error) {
        contact->body[0] = body[0];
        contact->body[1] = body[1];
        XMStoreFloat3(&contact->contactNormal, normal);
        XMStoreFloat3(&contact->contactPoint, (a_pos_world + b_pos_world) * 0.5f);
        contact->penetration = length - error;
        contact->friction = 1.0f;
        contact->restitution = 0;
        return 1;
    }

    return 0;
}
