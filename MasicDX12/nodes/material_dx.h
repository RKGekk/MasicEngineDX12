#pragma once

#include <DirectXMath.h>

struct MaterialDX {
    DirectX::XMFLOAT4 Diffuse;
    DirectX::XMFLOAT3 Ambient;
    DirectX::XMFLOAT3 Specular;
    DirectX::XMFLOAT3 Emissive;
    float Power;
};