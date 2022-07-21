#include "rigid_body.h"
#include <cmath>
#include <limits>
#include "../tools/math_utitity.h"

bool RigidBody::getAwake() const {
	return m_is_awake;
}

void RigidBody::setAwake(bool awake) {
    if (awake) {
        m_is_awake = true;
        m_motion = m_sleep_epsilon * 2.0f;
    }
    else {
        m_is_awake = false;
        m_velocity.x = 0.0f; m_velocity.y = 0.0f; m_velocity.z = 0.0f;
        m_rotation.x = 0.0f; m_rotation.y = 0.0f; m_rotation.z = 0.0f;
    }
}

bool RigidBody::getCanSleep() const {
	return m_can_sleep;
}

void RigidBody::setCanSleep(const bool canSleep) {
    m_can_sleep = canSleep;
    if (!canSleep && !m_is_awake) {
        setAwake();
    }
}

const DirectX::XMFLOAT3& RigidBody::getLastFrameAcceleration3f() const {
    return m_last_frame_acceleration;
}

DirectX::XMVECTOR RigidBody::getLastFrameAcceleration() const {
    return DirectX::XMLoadFloat3(&m_last_frame_acceleration);
}

void RigidBody::clearAccumulators() {
    m_force_accum.x = 0.0f;
    m_force_accum.y = 0.0f;
    m_force_accum.z = 0.0f;
    m_torque_accum.x = 0.0f;
    m_torque_accum.y = 0.0f;
    m_torque_accum.z = 0.0f;
}

void RigidBody::addForce3f(const DirectX::XMFLOAT3& force) {
    m_force_accum.x += force.x;
    m_force_accum.y += force.y;
    m_force_accum.z += force.z;
    m_is_awake = true;
}

void RigidBody::addForce(DirectX::FXMVECTOR force) {
    using namespace DirectX;
    XMVECTOR force_accum = XMLoadFloat3(&m_force_accum) + force;
    XMStoreFloat3(&m_force_accum, force_accum);
}

void RigidBody::addForceAtPoint3f(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& point) {
    using namespace DirectX;

    XMVECTOR pt = XMLoadFloat3(&point);
    pt -= XMLoadFloat3(&m_position);

    m_force_accum.x += force.x;
    m_force_accum.y += force.y;
    m_force_accum.z += force.z;

    XMVECTOR force_cross = XMVector3Cross(pt, XMLoadFloat3(&force));
    XMVECTOR torque_accum = XMLoadFloat3(&m_torque_accum) + force_cross;
    XMStoreFloat3(&m_torque_accum, torque_accum);

    m_is_awake = true;
}

void RigidBody::addForceAtPoint(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point) {
    using namespace DirectX;

    XMVECTOR pt = point;
    pt -= XMLoadFloat3(&m_position);

    XMVECTOR force_accum = XMLoadFloat3(&m_force_accum) + force;
    XMStoreFloat3(&m_force_accum, force_accum);

    XMVECTOR force_cross = XMVector3Cross(pt, force);
    XMVECTOR torque_accum = XMLoadFloat3(&m_torque_accum) + force_cross;
    XMStoreFloat3(&m_torque_accum, torque_accum);

    m_is_awake = true;
}

void RigidBody::addForceAtPoint(const DirectX::XMFLOAT3& force, DirectX::FXMVECTOR point) {
    addForceAtPoint(XMLoadFloat3(&force), point);
}

void RigidBody::addForceAtPoint(DirectX::FXMVECTOR force, const DirectX::XMFLOAT3& point) {
    using namespace DirectX;
    addForceAtPoint(force, XMLoadFloat3(&point));
}

void RigidBody::addForceAtBodyPoint3f(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& point) {
    using namespace DirectX;
    XMVECTOR pt = getPointInWorldSpace(point);
    addForceAtPoint(XMLoadFloat3(&force), pt);
}

void RigidBody::addForceAtBodyPoint(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point) {
    using namespace DirectX;
    XMVECTOR pt = getPointInWorldSpace(point);
    addForceAtPoint(force, pt);
}

void RigidBody::addForceAtBodyPoint(const DirectX::XMFLOAT3& force, DirectX::FXMVECTOR point) {
    using namespace DirectX;
    XMVECTOR pt = getPointInWorldSpace(point);
    addForceAtPoint(force, pt);
}

void RigidBody::addForceAtBodyPoint(DirectX::FXMVECTOR force, const DirectX::XMFLOAT3& point) {
    using namespace DirectX;
    XMVECTOR pt = getPointInWorldSpace(point);
    addForceAtPoint(force, pt);
}

void RigidBody::addTorque3f(const DirectX::XMFLOAT3& torque) {
    m_torque_accum.x += torque.x;
    m_torque_accum.y += torque.y;
    m_torque_accum.z += torque.z;

    m_is_awake = true;
}

void RigidBody::addTorque(DirectX::FXMVECTOR torque) {
    using namespace DirectX;
    XMVECTOR torque_accum = XMLoadFloat3(&m_torque_accum) + torque;
    XMStoreFloat3(&m_torque_accum, torque_accum);
}

void RigidBody::setAcceleration3f(const DirectX::XMFLOAT3& acceleration) {
    m_acceleration = acceleration;
}

void RigidBody::setAcceleration(DirectX::FXMVECTOR acceleration) {
    DirectX::XMStoreFloat3(&m_acceleration, acceleration);
}

void RigidBody::setAcceleration(float x, float y, float z) {
    m_acceleration.x = x;
    m_acceleration.y = y;
    m_acceleration.z = z;
}

const DirectX::XMFLOAT3& RigidBody::getAcceleration3f() const {
    return m_acceleration;
}

DirectX::XMVECTOR RigidBody::getAcceleration() const {
    return DirectX::XMLoadFloat3(&m_acceleration);
}

void RigidBody::calculateDerivedData() {
    using namespace DirectX;

    XMVECTOR orientation = XMQuaternionNormalize(XMLoadFloat4(&m_orientation));
    XMMATRIX orientationMatrix = XMMatrixRotationQuaternion(orientation);
    XMMATRIX transformMatrix = XMMatrixMultiply(orientationMatrix, XMMatrixTranslation(m_position.x, m_position.y, m_position.z));
    XMStoreFloat4x4(&m_transform_matrix, transformMatrix);

    XMMATRIX inverseInertiaTensor = XMLoadFloat3x3(&m_inverse_inertia_tensor);
    XMMATRIX inverseInertiaTensorInWorldSpace = XMMatrixMultiply(orientationMatrix, inverseInertiaTensor);
    XMMATRIX inverseInertiaTensorWorld = XMMatrixMultiply(inverseInertiaTensorInWorldSpace, XMMatrixTranspose(orientationMatrix));
    XMStoreFloat3x3(&m_inverse_inertia_tensor_world, inverseInertiaTensorWorld);
}

void RigidBody::integrate(float duration) {
    using namespace DirectX;

    if (!m_is_awake) { return; }

    XMVECTOR last_frame_acceleration = XMLoadFloat3(&m_acceleration) + XMLoadFloat3(&m_force_accum) * m_inverse_mass;
    XMStoreFloat3(&m_last_frame_acceleration, last_frame_acceleration);

    XMVECTOR velocity = XMLoadFloat3(&m_velocity) + last_frame_acceleration * duration;
    velocity *= std::powf(m_linear_damping, duration);
    XMStoreFloat3(&m_velocity, velocity);
    XMVECTOR position = XMLoadFloat3(&m_position) + velocity * duration;
    XMStoreFloat3(&m_position, position);

    XMVECTOR angularAcceleration = XMVector3TransformNormal(XMLoadFloat3(&m_torque_accum), XMLoadFloat3x3(&m_inverse_inertia_tensor_world));
    XMVECTOR rotation = XMLoadFloat3(&m_rotation) + angularAcceleration * duration;
    rotation *= std::powf(m_angular_damping, duration);
    XMStoreFloat3(&m_rotation, rotation);
    
    XMVECTOR orientation_old = XMLoadFloat4(&m_orientation);
    XMVECTOR orientation = orientation_old + XMQuaternionMultiply(XMVectorSetW(rotation, 0.0f) * duration, orientation_old) * 0.5f;
    XMStoreFloat4(&m_orientation, orientation);

    calculateDerivedData();

    clearAccumulators();

    if (m_can_sleep) {
        float currentMotion = XMVectorGetX(XMVector3Dot(velocity, velocity) + XMVector3Dot(rotation, rotation));

        float bias = std::powf(0.5f, duration);
        m_motion = bias * m_motion + (1.0f - bias) * currentMotion;

        if (m_motion < m_sleep_epsilon) {
            setAwake(false);
        }
        else if (m_motion > 10.0f * m_sleep_epsilon) {
            m_motion = 10.0f * m_sleep_epsilon;
        }
    }
}

void RigidBody::setMass(float mass) {
    m_inverse_mass = 1.0f / mass;
}

float RigidBody::getMass() const {
    if (DirectX::XMScalarNearEqual(m_inverse_mass, 0.0f, EPSILON)) {
        return std::numeric_limits<float>::max();
    }
    else {
        return 1.0f / m_inverse_mass;
    }
}

void RigidBody::setInverseMass(float inverseMass) {
    m_inverse_mass = inverseMass;
}

float RigidBody::getInverseMass() const {
    return m_inverse_mass;
}

bool RigidBody::hasFiniteMass() const {
    return m_inverse_mass >= EPSILON;
}

void RigidBody::setInertiaTensor(const DirectX::XMFLOAT3X3& inertiaTensor) {
    DirectX::XMStoreFloat3x3(&m_inverse_inertia_tensor, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat3x3(&inertiaTensor)));
}

void RigidBody::setInertiaTensor(DirectX::FXMMATRIX inertiaTensor) {
    DirectX::XMStoreFloat3x3(&m_inverse_inertia_tensor, DirectX::XMMatrixInverse(nullptr, inertiaTensor));
}

DirectX::XMMATRIX RigidBody::getInertiaTensor() const {
    return DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat3x3(&m_inverse_inertia_tensor));
}

DirectX::XMFLOAT3X3 RigidBody::getInertiaTensor3x3f() const {
    DirectX::XMFLOAT3X3 res;
    DirectX::XMStoreFloat3x3(&res, getInertiaTensor());
    return res;
}

DirectX::XMFLOAT3X3 RigidBody::getInertiaTensorWorld3x3f() const {
    DirectX::XMFLOAT3X3 res;
    DirectX::XMStoreFloat3x3(&res, getInertiaTensorWorld());
    return res;
}

DirectX::XMMATRIX RigidBody::getInertiaTensorWorld() const {
    return DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat3x3(&m_inverse_inertia_tensor_world));
}

void RigidBody::setInverseInertiaTensor3x3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor) {
    m_inverse_inertia_tensor = inverseInertiaTensor;
}

void RigidBody::setInverseInertiaTensor(DirectX::FXMMATRIX inverseInertiaTensor) {
    DirectX::XMStoreFloat3x3(&m_inverse_inertia_tensor, inverseInertiaTensor);
}

DirectX::XMMATRIX RigidBody::getInverseInertiaTensor() const {
    return DirectX::XMLoadFloat3x3(&m_inverse_inertia_tensor);
}

const DirectX::XMFLOAT3X3& RigidBody::getInverseInertiaTensor3x3f() const {
    return m_inverse_inertia_tensor;
}

DirectX::XMMATRIX RigidBody::getInverseInertiaTensorWorld() const {
    return DirectX::XMLoadFloat3x3(&m_inverse_inertia_tensor_world);
}

const DirectX::XMFLOAT3X3& RigidBody::getInverseInertiaTensorWorld3x3f() const {
    return m_inverse_inertia_tensor_world;
}

void RigidBody::setDamping(float linearDamping, float angularDamping) {
    m_linear_damping = linearDamping;
    m_angular_damping = angularDamping;
}

void RigidBody::setLinearDamping(float linearDamping) {
    m_linear_damping = linearDamping;
}

float RigidBody::getLinearDamping() const {
    return m_linear_damping;
}

void RigidBody::setAngularDamping(float angularDamping) {
    m_angular_damping = angularDamping;
}

float RigidBody::getAngularDamping() const {
    return m_angular_damping;
}

void RigidBody::setPosition3f(const DirectX::XMFLOAT3& position) {
    m_position = position;
}

void RigidBody::setPosition(DirectX::FXMVECTOR position) {
    DirectX::XMStoreFloat3(&m_position, position);
}

void RigidBody::setPosition(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

DirectX::XMVECTOR RigidBody::getPosition() const {
    return DirectX::XMLoadFloat3(&m_position);
}

const DirectX::XMFLOAT3& RigidBody::getPosition3f() const {
    return m_position;
}

void RigidBody::setOrientation4f(const DirectX::XMFLOAT4& orientation) {
    DirectX::XMStoreFloat4(&m_orientation, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&orientation)));
}

void RigidBody::setOrientation(DirectX::FXMVECTOR orientation) {
    DirectX::XMStoreFloat4(&m_orientation, DirectX::XMQuaternionNormalize(orientation));
}

void RigidBody::setOrientation(float r, float i, float j, float k) {
    DirectX::XMStoreFloat4(&m_orientation, DirectX::XMQuaternionNormalize(DirectX::XMVectorSet(i, j, k, r)));
}

DirectX::XMVECTOR RigidBody::getOrientationQ() const {
    return DirectX::XMLoadFloat4(&m_orientation);
}

const DirectX::XMFLOAT4& RigidBody::getOrientation4f() const {
    return m_orientation;
}

DirectX::XMFLOAT3X3 RigidBody::getOrientation3x3f() const {
    return {
        m_transform_matrix._11, m_transform_matrix._12, m_transform_matrix._13,
        m_transform_matrix._21, m_transform_matrix._22, m_transform_matrix._23,
        m_transform_matrix._31, m_transform_matrix._32, m_transform_matrix._33
    };
}

DirectX::XMMATRIX RigidBody::getOrientation() const {
    DirectX::XMFLOAT3X3 res = getOrientation3x3f();
    return DirectX::XMLoadFloat3x3(&res);
}

DirectX::XMMATRIX RigidBody::getTransform() const {
    return DirectX::XMLoadFloat4x4(&m_transform_matrix);
}

const DirectX::XMFLOAT4X4& RigidBody::getTransform4x4f() const {
    return m_transform_matrix;
}

DirectX::XMVECTOR RigidBody::getPointInLocalSpace(const DirectX::XMFLOAT3& point) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSet(point.x, point.y, point.z, 1.0f), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_transform_matrix)));
}

DirectX::XMVECTOR RigidBody::getPointInLocalSpace(DirectX::FXMVECTOR point) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSetW(point, 1.0f), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_transform_matrix)));
}

DirectX::XMFLOAT3 RigidBody::getPointInLocalSpace3f(const DirectX::XMFLOAT3& point) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getPointInLocalSpace(point));
    return res;
}

DirectX::XMFLOAT3 RigidBody::getPointInLocalSpace3f(DirectX::FXMVECTOR point) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getPointInLocalSpace(point));
    return res;
}

DirectX::XMVECTOR RigidBody::getPointInWorldSpace(DirectX::FXMVECTOR point) const {
    using namespace DirectX;
    return XMVector4Transform(point, XMLoadFloat4x4(&m_transform_matrix));
}

DirectX::XMVECTOR RigidBody::getPointInWorldSpace(const DirectX::XMFLOAT3& point) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSet(point.x, point.y, point.z, 1.0f), XMLoadFloat4x4(&m_transform_matrix));
}

DirectX::XMFLOAT3 RigidBody::getPointInWorldSpace3f(DirectX::FXMVECTOR point) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getPointInWorldSpace(point));
    return res;
}

DirectX::XMFLOAT3 RigidBody::getPointInWorldSpace3f(const DirectX::XMFLOAT3& point) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getPointInWorldSpace(point));
    return res;
}

DirectX::XMVECTOR RigidBody::getDirectionInLocalSpace(DirectX::FXMVECTOR direction) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSetW(direction, 0.0f), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_transform_matrix)));
}

DirectX::XMVECTOR RigidBody::getDirectionInLocalSpace(const DirectX::XMFLOAT3& direction) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSet(direction.x, direction.y, direction.z, 0.0f), XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_transform_matrix)));
}

DirectX::XMFLOAT3 RigidBody::getDirectionInLocalSpace3f(DirectX::FXMVECTOR direction) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getDirectionInLocalSpace(direction));
    return res;
}

DirectX::XMFLOAT3 RigidBody::getDirectionInLocalSpace3f(const DirectX::XMFLOAT3& direction) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getDirectionInLocalSpace(direction));
    return res;
}

DirectX::XMVECTOR RigidBody::getDirectionInWorldSpace(DirectX::FXMVECTOR direction) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSetW(direction, 0.0f), XMLoadFloat4x4(&m_transform_matrix));
}

DirectX::XMVECTOR RigidBody::getDirectionInWorldSpace(const DirectX::XMFLOAT3& direction) const {
    using namespace DirectX;
    return XMVector4Transform(XMVectorSet(direction.x, direction.y, direction.z, 0.0f), XMLoadFloat4x4(&m_transform_matrix));
}

DirectX::XMFLOAT3 RigidBody::getDirectionInWorldSpace3f(DirectX::FXMVECTOR direction) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getDirectionInWorldSpace(direction));
    return res;
}

DirectX::XMFLOAT3 RigidBody::getDirectionInWorldSpace3f(const DirectX::XMFLOAT3& direction) const {
    DirectX::XMFLOAT3 res;
    DirectX::XMStoreFloat3(&res, getDirectionInWorldSpace(direction));
    return res;
}

void RigidBody::setVelocity(DirectX::FXMVECTOR velocity) {
    DirectX::XMStoreFloat3(&m_velocity, velocity);
}

void RigidBody::setVelocity3f(const DirectX::XMFLOAT3& velocity) {
    m_velocity = velocity;
}

void RigidBody::setVelocity(float x, float y, float z) {
    m_velocity.x = x;
    m_velocity.y = y;
    m_velocity.z = z;
}

DirectX::XMVECTOR RigidBody::getVelocity() const {
    return DirectX::XMLoadFloat3(&m_velocity);
}

const DirectX::XMFLOAT3& RigidBody::getVelocity3f() const {
    return m_velocity;
}

void RigidBody::addVelocity3f(const DirectX::XMFLOAT3& deltaVelocity) {
    m_velocity.x += deltaVelocity.x;
    m_velocity.y += deltaVelocity.y;
    m_velocity.z += deltaVelocity.z;
}

void RigidBody::addVelocity(DirectX::FXMVECTOR deltaVelocity) {
    using namespace DirectX;
    XMVECTOR velocity = XMLoadFloat3(&m_velocity) + deltaVelocity;
    XMStoreFloat3(&m_velocity, velocity);
}

void RigidBody::setRotation3f(const DirectX::XMFLOAT3& rotation) {
    m_rotation = rotation;
}

void RigidBody::setRotation(DirectX::FXMVECTOR rotation) {
    DirectX::XMStoreFloat3(&m_rotation, rotation);
}

void RigidBody::setRotation(float x, float y, float z) {
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}

DirectX::XMVECTOR RigidBody::getRotation() const {
    return DirectX::XMLoadFloat3(&m_rotation);
}

const DirectX::XMFLOAT3& RigidBody::getRotation3f() const {
    return m_rotation;
}

void RigidBody::addRotation(DirectX::XMVECTOR deltaRotation) {
    using namespace DirectX;
    XMVECTOR rotation = XMLoadFloat3(&m_rotation) + deltaRotation;
}

void RigidBody::addRotation3f(const DirectX::XMFLOAT3& deltaRotation) {
    m_rotation.x += deltaRotation.x;
    m_rotation.y += deltaRotation.y;
    m_rotation.z += deltaRotation.z;
}



