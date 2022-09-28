#pragma once

#include "actor_component.h"

#include <pugixml/pugixml.hpp>
#include <DirectXMath.h>

class TransformComponent : public ActorComponent {
protected:
    virtual void VRegisterEvents();

private:
    DirectX::XMFLOAT4X4 m_transform;

    DirectX::XMFLOAT4 m_forward;
    DirectX::XMFLOAT4 m_up;
    DirectX::XMFLOAT4 m_right;

    DirectX::XMFLOAT4 m_scale;

    uint32_t m_generation;

public:
    static const std::string g_Name;

    TransformComponent();
    TransformComponent(const pugi::xml_node& data);
    virtual ~TransformComponent();

    virtual bool VInit(const pugi::xml_node& data) override;
    virtual const std::string& VGetName() const override;
    virtual pugi::xml_node VGenerateXml() override;

    uint32_t GetGeneration();

    // transform functions
    const DirectX::XMFLOAT4X4& GetTransform4x4f() const;
    DirectX::XMFLOAT4X4 GetTransform4x4T() const;
    DirectX::XMMATRIX GetTransform() const;
    DirectX::XMMATRIX GetTransformT() const;
    DirectX::XMFLOAT4X4 GetInvTransform4x4f() const;
    DirectX::XMMATRIX GetInvTransform() const;

    DirectX::XMFLOAT4X4 GetFullTransform4x4f() const;
    DirectX::XMFLOAT4X4 GetFullTransform4x4T() const;
    DirectX::XMMATRIX GetFullTransform() const;
    DirectX::XMMATRIX GetFullTransformT() const;
    DirectX::XMFLOAT4X4 GetFullInvTransform4x4f() const;
    DirectX::XMMATRIX GetFullInvTransform() const;

    void SetTransform(const DirectX::XMFLOAT4X4& newTransform);
    void SetTransform(DirectX::FXMMATRIX newTransform);

    DirectX::XMFLOAT3 GetPosition3f() const;
    DirectX::XMFLOAT4 GetPosition4f() const;
    DirectX::XMVECTOR GetPosition() const;

    DirectX::XMFLOAT3 GetScale3f() const;
    DirectX::XMVECTOR GetScale() const;

    void SetPosition3f(const DirectX::XMFLOAT3& pos);
    void SetPosition4f(const DirectX::XMFLOAT4& pos);
    void SetPosition4x4f(const DirectX::XMFLOAT4X4& pos);
    void SetPosition3(DirectX::FXMVECTOR pos);
    void SetPosition4(DirectX::FXMVECTOR pos);

    void SetYawPitchRoll3f(const DirectX::XMFLOAT3& ypr);
    void SetYawPitchRollDeg3f(const DirectX::XMFLOAT3& ypr);

    void SetScale3f(const DirectX::XMFLOAT3& sclae);
    void SetScale4f(const DirectX::XMFLOAT4& sclae);
    void SetScale(DirectX::FXMVECTOR scale);

    DirectX::XMFLOAT3 GetLookAt3f() const;
    DirectX::XMVECTOR GetLookAt() const;

    DirectX::XMFLOAT3 GetYawPitchRoll3f() const;
    DirectX::XMVECTOR GetYawPitchRoll() const;

    DirectX::XMFLOAT3 GetForward3f() const;
    DirectX::XMFLOAT4 GetForward4f() const;
    DirectX::XMVECTOR GetForward() const;

    DirectX::XMFLOAT3 GetUp3f() const;
    DirectX::XMFLOAT4 GetUp4f() const;
    DirectX::XMVECTOR GetUp() const;

    DirectX::XMFLOAT3 GetRight3f() const;
    DirectX::XMFLOAT4 GetRight4f() const;
    DirectX::XMVECTOR GetRight() const;

private:
    bool Init(const pugi::xml_node& data);

    const DirectX::XMFLOAT4 DEFAULT_FORWARD_VECTOR = { 0.0f, 0.0f, 1.0f, 0.0f };
    const DirectX::XMFLOAT4 DEFAULT_UP_VECTOR = { 0.0f, 1.0f, 0.0f, 0.0f };
    const DirectX::XMFLOAT4 DEFAULT_RIGHT_VECTOR = { 1.0f, 0.0f, 0.0f, 0.0f };
    const float EPSILON = 0.001f;
};
