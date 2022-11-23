#include "transform_component.h"

#include "../tools/string_utility.h"

const std::string TransformComponent::g_Name = "TransformComponent";

void TransformComponent::VRegisterEvents() {}

TransformComponent::TransformComponent() {
    DirectX::XMStoreFloat4x4(&m_transform, DirectX::XMMatrixIdentity());

    m_generation = 0u;

    m_forward = DEFAULT_FORWARD_VECTOR;
    m_up = DEFAULT_UP_VECTOR;
    m_right = DEFAULT_RIGHT_VECTOR;
}

TransformComponent::TransformComponent(const pugi::xml_node& data) {
    Init(data);
}

TransformComponent::~TransformComponent() {}

const std::string& TransformComponent::VGetName() const {
    return g_Name;
}

pugi::xml_node TransformComponent::VGenerateXml() {
    return pugi::xml_node();
}

uint32_t TransformComponent::GetGeneration() {
    return m_generation;
}

const DirectX::XMFLOAT4X4& TransformComponent::GetTransform4x4f() const {
    return m_transform;
}

DirectX::XMFLOAT4X4 TransformComponent::GetTransform4x4T() const {
    DirectX::XMFLOAT4X4 res = {};
    DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_transform)));
    return res;
}

DirectX::XMMATRIX TransformComponent::GetTransform() const {
    return DirectX::XMLoadFloat4x4(&m_transform);
}

DirectX::XMMATRIX TransformComponent::GetTransformT() const {
    return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_transform));
}

DirectX::XMFLOAT4X4 TransformComponent::GetInvTransform4x4f() const {
    DirectX::XMFLOAT4X4 res = {};
    DirectX::XMStoreFloat4x4(&res, GetInvTransform());
    return res;
}

DirectX::XMMATRIX TransformComponent::GetInvTransform() const {
    return DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&m_transform));
}

void TransformComponent::SetTransform(const DirectX::XMFLOAT4X4& newTransform) {
    m_transform = newTransform;
    ++m_generation;
}

void TransformComponent::SetTransform(DirectX::FXMMATRIX newTransform) {
    DirectX::XMStoreFloat4x4(&m_transform, newTransform);
    ++m_generation;
}

DirectX::XMFLOAT3 TransformComponent::GetTranslation3f() const {
    return DirectX::XMFLOAT3(m_transform._41, m_transform._42, m_transform._43);
}

DirectX::XMFLOAT4 TransformComponent::GetTranslation4f() const {
    return DirectX::XMFLOAT4(m_transform._41, m_transform._42, m_transform._43, m_transform._44);
}

DirectX::XMVECTOR TransformComponent::GetTranslation() const {
    return DirectX::XMVectorSet(m_transform._41, m_transform._42, m_transform._43, 1.0f);
}

void TransformComponent::SetTranslation3f(const DirectX::XMFLOAT3& pos) {
    m_transform._41 = pos.x;
    m_transform._42 = pos.y;
    m_transform._43 = pos.z;
    m_transform._44 = 1.0f;
    ++m_generation;
}

void TransformComponent::SetTranslation4f(const DirectX::XMFLOAT4& pos) {
    m_transform._41 = pos.x;
    m_transform._42 = pos.y;
    m_transform._43 = pos.z;
    m_transform._44 = pos.w;
    ++m_generation;
}

void TransformComponent::SetTranslation4x4f(const DirectX::XMFLOAT4X4& pos) {
    m_transform._41 = pos._41;
    m_transform._42 = pos._42;
    m_transform._43 = pos._43;
    m_transform._44 = pos._44;
    ++m_generation;
}

void TransformComponent::SetTranslation3(DirectX::FXMVECTOR pos) {
    DirectX::XMFLOAT3 temp;
    DirectX::XMStoreFloat3(&temp, pos);
    m_transform._41 = temp.x;
    m_transform._42 = temp.y;
    m_transform._43 = temp.z;
    m_transform._44 = 1.0f;
    ++m_generation;
}

void TransformComponent::SetTranslation4(DirectX::FXMVECTOR pos) {
    DirectX::XMFLOAT4 temp;
    DirectX::XMStoreFloat4(&temp, pos);
    m_transform._41 = temp.x;
    m_transform._42 = temp.y;
    m_transform._43 = temp.z;
    m_transform._44 = temp.w;
    ++m_generation;
}

DirectX::XMFLOAT3 TransformComponent::GetLookAt3f() const {
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, GetLookAt());

    return result;
}

DirectX::XMVECTOR TransformComponent::GetLookAt() const {
    DirectX::XMFLOAT4X4 justRot4x4 = m_transform;
    justRot4x4._41 = 0.0f;
    justRot4x4._42 = 0.0f;
    justRot4x4._43 = 0.0f;
    justRot4x4._44 = 1.0f;
    DirectX::XMMATRIX justRot = DirectX::XMLoadFloat4x4(&justRot4x4);

    DirectX::XMVECTOR forward = DirectX::XMLoadFloat4(&m_forward);
    DirectX::XMVECTOR out = DirectX::XMVector4Transform(forward, justRot);

    return out;
}

DirectX::XMFLOAT3 TransformComponent::GetForward3f() const {
    return DirectX::XMFLOAT3(m_forward.x, m_forward.y, m_forward.z);
}

DirectX::XMFLOAT4 TransformComponent::GetForward4f() const {
    return m_forward;
}

DirectX::XMVECTOR TransformComponent::GetForward() const {
    DirectX::XMVECTOR directionXM = DirectX::XMLoadFloat4(&m_forward);
    return directionXM;
}

DirectX::XMFLOAT3 TransformComponent::GetUp3f() const {
    return DirectX::XMFLOAT3(m_up.x, m_up.y, m_up.z);
}

DirectX::XMFLOAT4 TransformComponent::GetUp4f() const {
    return m_up;
}

DirectX::XMVECTOR TransformComponent::GetUp() const {
    DirectX::XMVECTOR directionXM = DirectX::XMLoadFloat4(&m_up);
    return directionXM;
}

DirectX::XMFLOAT3 TransformComponent::GetRight3f() const {
    return DirectX::XMFLOAT3(m_right.x, m_right.y, m_right.z);
}

DirectX::XMFLOAT4 TransformComponent::GetRight4f() const {
    return m_right;
}

DirectX::XMVECTOR TransformComponent::GetRight() const {
    DirectX::XMVECTOR directionXM = DirectX::XMLoadFloat4(&m_right);
    return directionXM;
}

bool TransformComponent::Init(const pugi::xml_node& data) {
    DirectX::XMFLOAT3 position = posfromattr3f(data.child("Position"), GetTranslation3f());
    DirectX::XMFLOAT3 yawPitchRoll = anglesfromattr3f(data.child("YawPitchRoll"), GetYawPitchRoll3f());
    DirectX::XMFLOAT3 scale = posfromattr3f(data.child("Scale"), {1.0f, 1.0f, 1.0f});

    DirectX::XMMATRIX translation_xm = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    DirectX::XMMATRIX rotation_xm = DirectX::XMMatrixRotationRollPitchYaw(yawPitchRoll.y, yawPitchRoll.x, yawPitchRoll.z);
    DirectX::XMMATRIX scale_xm = DirectX::XMMatrixScaling(scale.y, scale.x, scale.z);

    DirectX::XMStoreFloat4x4(
        &m_transform,
        DirectX::XMMatrixMultiply(
            DirectX::XMMatrixMultiply(
                scale_xm,
                rotation_xm
            ),
            translation_xm
        )
    );

    ++m_generation;

    return true;
}

DirectX::XMFLOAT3 TransformComponent::GetYawPitchRoll3f() const {

    float pitch = DirectX::XMScalarASin(-m_transform._32);

    DirectX::XMVECTOR from(DirectX::XMVectorSet(m_transform._12, m_transform._31, 0.0f, 0.0f));
    DirectX::XMVECTOR to(DirectX::XMVectorSet(m_transform._22, m_transform._33, 0.0f, 0.0f));
    DirectX::XMVECTOR res(DirectX::XMVectorATan2(from, to));

    float roll = DirectX::XMVectorGetX(res);
    float yaw = DirectX::XMVectorGetY(res);

    return DirectX::XMFLOAT3(yaw, pitch, roll);
}

DirectX::XMVECTOR TransformComponent::GetYawPitchRoll() const {

    DirectX::XMFLOAT3 yawPitchRoll = GetYawPitchRoll3f();
    return DirectX::XMLoadFloat3(&yawPitchRoll);
}

bool TransformComponent::VInit(const pugi::xml_node& pData) {
    return Init(pData);
}