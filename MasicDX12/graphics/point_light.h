#pragma once

#include <DirectXMath.h>

struct PointLight {
    PointLight() : PositionWS(0.0f, 0.0f, 0.0f, 1.0f), PositionVS(0.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Ambient(0.01f), ConstantAttenuation(1.0f), LinearAttenuation(0.0f), QuadraticAttenuation(0.0f) {}

    // Light position in world space.
    DirectX::XMFLOAT4 PositionWS;
    //----------------------------------- (16 byte boundary)
    
    // Light position in view space.
    DirectX::XMFLOAT4 PositionVS;  
    //----------------------------------- (16 byte boundary)

    DirectX::XMFLOAT4 Color;
    //----------------------------------- (16 byte boundary)

    float Ambient;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 4 = 64 bytes
};