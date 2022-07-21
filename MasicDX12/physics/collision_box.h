#pragma once

#include <DirectXMath.h>

#include "collision_primitive.h"

class CollisionBox : public CollisionPrimitive {
public:
    DirectX::XMFLOAT3 halfSize;
};