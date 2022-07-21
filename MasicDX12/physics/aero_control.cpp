#include "aero_control.h"

DirectX::XMFLOAT3X3 AeroControl::getTensor3x3f() {
    using namespace DirectX;
    
    XMFLOAT3X3 res;
    XMStoreFloat3x3(&res, getTensor());
    return res;
}

DirectX::XMMATRIX AeroControl::getTensor() {
    using namespace DirectX;
    if (m_controlSetting <= -1.0f) {
        return XMLoadFloat3x3(&m_minTensor);
    }
    else if (m_controlSetting >= 1.0f) {
        return XMLoadFloat3x3(&m_maxTensor);
    }
    else if (m_controlSetting < 0) {
        XMMATRIX minTensorXM = XMLoadFloat3x3(&m_minTensor);
        XMMATRIX tensorXM = XMLoadFloat3x3(&m_tensor);
        XMMATRIX tensorNewXM;
        tensorNewXM.r[0] = XMVectorLerp(minTensorXM.r[0], tensorXM.r[0], m_controlSetting + 1.0f);
        tensorNewXM.r[1] = XMVectorLerp(minTensorXM.r[1], tensorXM.r[1], m_controlSetting + 1.0f);
        tensorNewXM.r[2] = XMVectorLerp(minTensorXM.r[2], tensorXM.r[2], m_controlSetting + 1.0f);
        return tensorNewXM;
    }
    else if (m_controlSetting > 0) {
        XMMATRIX tensorXM = XMLoadFloat3x3(&m_tensor);
        XMMATRIX maxTensorXM = XMLoadFloat3x3(&m_maxTensor);
        XMMATRIX tensorNewXM;
        tensorNewXM.r[0] = XMVectorLerp(tensorXM.r[0], maxTensorXM.r[0], m_controlSetting);
        tensorNewXM.r[1] = XMVectorLerp(tensorXM.r[1], maxTensorXM.r[1], m_controlSetting);
        tensorNewXM.r[2] = XMVectorLerp(tensorXM.r[2], maxTensorXM.r[2], m_controlSetting);
        return tensorNewXM;
    }
    else {
        return XMLoadFloat3x3(&m_tensor);
    }
}

AeroControl::AeroControl(const DirectX::XMFLOAT3X3& base, const DirectX::XMFLOAT3X3& min, const DirectX::XMFLOAT3X3& max, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3* windspeed) : Aero(base, position, windspeed) {
    m_minTensor = min;
    m_maxTensor = max;
    m_controlSetting = 0.0f;
}

AeroControl::AeroControl(DirectX::FXMMATRIX base, DirectX::CXMMATRIX min, DirectX::CXMMATRIX max, DirectX::XMVECTOR position, DirectX::XMFLOAT3* windspeed) : Aero(base, position, windspeed) {
    DirectX::XMStoreFloat3x3(&m_minTensor, min);
    DirectX::XMStoreFloat3x3(&m_maxTensor, max);
    m_controlSetting = 0.0f;
}

void AeroControl::setControl(float value) {
    m_controlSetting = value;
}

void AeroControl::updateForce(RigidBody* body, float duration) {
    updateForceFromTensor(body, duration, getTensor());
}
