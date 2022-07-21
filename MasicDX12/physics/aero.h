#pragma once

#include <DirectXMath.h>

#include "force_generator.h"

class Aero : public ForceGenerator {
protected:
    DirectX::XMFLOAT3X3 m_tensor;
    DirectX::XMFLOAT3 m_position;
    const DirectX::XMFLOAT3* m_windspeed;

public:
    Aero(const DirectX::XMFLOAT3X3& tensor, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed);
    Aero(DirectX::FXMMATRIX tensor, const DirectX::CXMVECTOR position, const DirectX::XMFLOAT3* windspeed);

    virtual void updateForce(RigidBody* body, float duration) override;

protected:
    void updateForceFromTensor(RigidBody* body, float duration, const DirectX::XMFLOAT3X3& tensor);
    void updateForceFromTensor(RigidBody* body, float duration, DirectX::FXMMATRIX tensor);
};