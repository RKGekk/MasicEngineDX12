#pragma once

#include <DirectXMath.h>

class CollisionPlane {
public:
    DirectX::XMFLOAT3 direction;
    float offset;
};