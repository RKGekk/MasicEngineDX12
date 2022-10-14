#pragma once

#include <DirectXMath.h>

struct DirectionalLight {
    DirectionalLight() : DirectionWS(0.0f, 0.0f, 1.0f, 0.0f), DirectionVS(0.0f, 0.0f, 1.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Ambient(0.01f, 0.01f, 0.01f) {}

    // Light direction in world space.
    DirectX::XMFLOAT4 DirectionWS;
    //----------------------------------- (16 byte boundary)

    // Light direction in view space.
    DirectX::XMFLOAT4 DirectionVS;
    //----------------------------------- (16 byte boundary)

    DirectX::XMFLOAT4 Color;
    //----------------------------------- (16 byte boundary)

    DirectX::XMFLOAT3 Ambient;
    float Padding;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 4 = 64 bytes
};