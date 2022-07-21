#pragma once

#include <DirectXMath.h>

#include "rigid_body.h"
#include "aero.h"

class AngledAero : public Aero {
    DirectX::XMFLOAT4 m_orientation;

public:
    AngledAero(const DirectX::XMFLOAT3X3& tensor, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed);
    AngledAero(DirectX::FXMMATRIX tensor, DirectX::FXMVECTOR position, const DirectX::XMFLOAT3* windspeed);

    void setOrientation(const DirectX::XMFLOAT4& quat);
    void setOrientation(DirectX::XMVECTOR quat);
    virtual void updateForce(RigidBody* body, float duration) override;
};