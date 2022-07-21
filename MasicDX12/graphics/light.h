#pragma once

#include <DirectXMath.h>

struct PointLight {
    PointLight()
        : PositionWS(0.0f, 0.0f, 0.0f, 1.0f)
        , PositionVS(0.0f, 0.0f, 0.0f, 1.0f)
        , Color(1.0f, 1.0f, 1.0f, 1.0f)
        , Ambient(0.01f)
        , ConstantAttenuation(1.0f)
        , LinearAttenuation(0.0f)
        , QuadraticAttenuation(0.0f) {}

    DirectX::XMFLOAT4 PositionWS;  // Light position in world space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 PositionVS;  // Light position in view space.
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

struct SpotLight {
    SpotLight()
        : PositionWS(0.0f, 0.0f, 0.0f, 1.0f)
        , PositionVS(0.0f, 0.0f, 0.0f, 1.0f)
        , DirectionWS(0.0f, 0.0f, 1.0f, 0.0f)
        , DirectionVS(0.0f, 0.0f, 1.0f, 0.0f)
        , Color(1.0f, 1.0f, 1.0f, 1.0f)
        , Ambient(0.01f)
        , SpotAngle(DirectX::XM_PIDIV2)
        , ConstantAttenuation(1.0f)
        , LinearAttenuation(0.0f)
        , QuadraticAttenuation(0.0f) {}

    DirectX::XMFLOAT4 PositionWS;  // Light position in world space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 PositionVS;  // Light position in view space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 DirectionWS;  // Light direction in world space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 DirectionVS;  // Light direction in view space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 Color;
    //----------------------------------- (16 byte boundary)
    float Ambient;
    float SpotAngle;
    float ConstantAttenuation;
    float LinearAttenuation;
    //----------------------------------- (16 byte boundary)
    float QuadraticAttenuation;
    float Padding[3];
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 7 = 112 bytes
};

struct DirectionalLight {
    DirectionalLight()
        : DirectionWS(0.0f, 0.0f, 1.0f, 0.0f)
        , DirectionVS(0.0f, 0.0f, 1.0f, 0.0f)
        , Color(1.0f, 1.0f, 1.0f, 1.0f)
        , Ambient(0.01f) {}

    DirectX::XMFLOAT4 DirectionWS;  // Light direction in world space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 DirectionVS;  // Light direction in view space.
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4 Color;
    //----------------------------------- (16 byte boundary)
    float Ambient;
    float Padding[3];
    //----------------------------------- (16 byte boundary)
    // Total:                              16 * 4 = 64 bytes
};