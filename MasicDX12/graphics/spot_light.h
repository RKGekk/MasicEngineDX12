#pragma once

#include <DirectXMath.h>

struct SpotLight {
    SpotLight() : PositionWS(0.0f, 0.0f, 0.0f, 1.0f), PositionVS(0.0f, 0.0f, 0.0f, 1.0f), DirectionWS(0.0f, 0.0f, 1.0f, 0.0f), DirectionVS(0.0f, 0.0f, 1.0f, 0.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Ambient(0.01f, 0.01f, 0.01f), SpotAngle(DirectX::XM_PIDIV2), ConstantAttenuation(1.0f), LinearAttenuation(0.0f), QuadraticAttenuation(0.0f) {}

    // Light position in world space.
    DirectX::XMFLOAT4 PositionWS;
    //----------------------------------- (16 byte boundary)
    // 
    // Light position in view space.
    DirectX::XMFLOAT4 PositionVS;
    //----------------------------------- (16 byte boundary)

    // Light direction in world space.
    DirectX::XMFLOAT4 DirectionWS;
    //----------------------------------- (16 byte boundary)

    // Light direction in view space.
    DirectX::XMFLOAT4 DirectionVS;
    //----------------------------------- (16 byte boundary)

    DirectX::XMFLOAT4 Color;
    //----------------------------------- (16 byte boundary)

    float SpotAngle;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    //----------------------------------- (16 byte boundary)

    DirectX::XMFLOAT3 Ambient;
    float Padding;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 7 = 112 bytes
};