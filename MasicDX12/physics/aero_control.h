#pragma once

#include <DirectXMath.h>

#include "aero.h"

class AeroControl : public Aero {
protected:
    DirectX::XMFLOAT3X3 m_maxTensor;
    DirectX::XMFLOAT3X3 m_minTensor;
    float m_controlSetting;

private:
    DirectX::XMFLOAT3X3 getTensor3x3f();
    DirectX::XMMATRIX getTensor();

public:
    AeroControl(const DirectX::XMFLOAT3X3& base, const DirectX::XMFLOAT3X3& min, const DirectX::XMFLOAT3X3& max, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed);
    AeroControl(DirectX::FXMMATRIX base, DirectX::CXMMATRIX min, DirectX::CXMMATRIX max, DirectX::XMVECTOR position, DirectX::XMFLOAT3* windspeed);

    void setControl(float value);
    virtual void updateForce(RigidBody* body, float duration) override;
};