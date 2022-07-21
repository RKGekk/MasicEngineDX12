#pragma once

#include <DirectXMath.h>

class RigidBody {
protected:
    const float m_sleep_epsilon = 0.3f;
    float m_inverse_mass;
    DirectX::XMFLOAT3X3 m_inverse_inertia_tensor;
    float m_linear_damping;
    float m_angular_damping;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_orientation;
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3X3 m_inverse_inertia_tensor_world;
    float m_motion;
    bool m_is_awake;
    bool m_can_sleep;
    DirectX::XMFLOAT4X4 m_transform_matrix;
    DirectX::XMFLOAT3 m_force_accum;
    DirectX::XMFLOAT3 m_torque_accum;
    DirectX::XMFLOAT3 m_acceleration;
    DirectX::XMFLOAT3 m_last_frame_acceleration;

public:
    void calculateDerivedData();
    void integrate(float duration);

    void setMass(float mass);
    float getMass() const;

    void setInverseMass(float inverseMass);
    float getInverseMass() const;
    bool hasFiniteMass() const;

    void setInertiaTensor(const DirectX::XMFLOAT3X3& inertiaTensor);
    void setInertiaTensor(DirectX::FXMMATRIX inertiaTensor);
    DirectX::XMMATRIX getInertiaTensor() const;
    DirectX::XMFLOAT3X3 getInertiaTensor3x3f() const;

    DirectX::XMFLOAT3X3 getInertiaTensorWorld3x3f() const;
    DirectX::XMMATRIX getInertiaTensorWorld() const;

    void setInverseInertiaTensor3x3f(const DirectX::XMFLOAT3X3& inverseInertiaTensor);
    void setInverseInertiaTensor(DirectX::FXMMATRIX inverseInertiaTensor);
    DirectX::XMMATRIX getInverseInertiaTensor() const;
    const DirectX::XMFLOAT3X3& getInverseInertiaTensor3x3f() const;

    DirectX::XMMATRIX getInverseInertiaTensorWorld() const;
    const DirectX::XMFLOAT3X3& getInverseInertiaTensorWorld3x3f() const;

    void setDamping(float linearDamping, float angularDamping);
    void setLinearDamping(float linearDamping);
    float getLinearDamping() const;
    void setAngularDamping(float angularDamping);
    float getAngularDamping() const;

    void setPosition3f(const DirectX::XMFLOAT3& position);
    void setPosition(DirectX::FXMVECTOR position);
    void setPosition(float x, float y, float z);
    DirectX::XMVECTOR getPosition() const;
    const DirectX::XMFLOAT3& getPosition3f() const;

    void setOrientation4f(const DirectX::XMFLOAT4& orientation);
    void setOrientation(DirectX::FXMVECTOR orientation);
    void setOrientation(float r, float i, float j, float k);
    DirectX::XMVECTOR getOrientationQ() const;
    const DirectX::XMFLOAT4& getOrientation4f() const;
    DirectX::XMFLOAT3X3 getOrientation3x3f() const;
    DirectX::XMMATRIX getOrientation() const;

    DirectX::XMMATRIX getTransform() const;
    const DirectX::XMFLOAT4X4& getTransform4x4f() const;

    DirectX::XMVECTOR getPointInLocalSpace(const DirectX::XMFLOAT3& point) const;
    DirectX::XMVECTOR getPointInLocalSpace(DirectX::FXMVECTOR point) const;
    DirectX::XMFLOAT3 getPointInLocalSpace3f(const DirectX::XMFLOAT3& point) const;
    DirectX::XMFLOAT3 getPointInLocalSpace3f(DirectX::FXMVECTOR point) const;

    DirectX::XMVECTOR getPointInWorldSpace(DirectX::FXMVECTOR point) const;
    DirectX::XMVECTOR getPointInWorldSpace(const DirectX::XMFLOAT3& point) const;
    DirectX::XMFLOAT3 getPointInWorldSpace3f(DirectX::FXMVECTOR point) const;
    DirectX::XMFLOAT3 getPointInWorldSpace3f(const DirectX::XMFLOAT3& point) const;

    DirectX::XMVECTOR getDirectionInLocalSpace(DirectX::FXMVECTOR direction) const;
    DirectX::XMVECTOR getDirectionInLocalSpace(const DirectX::XMFLOAT3& direction) const;
    DirectX::XMFLOAT3 getDirectionInLocalSpace3f(DirectX::FXMVECTOR direction) const;
    DirectX::XMFLOAT3 getDirectionInLocalSpace3f(const DirectX::XMFLOAT3& direction) const;

    DirectX::XMVECTOR getDirectionInWorldSpace(DirectX::FXMVECTOR direction) const;
    DirectX::XMVECTOR getDirectionInWorldSpace(const DirectX::XMFLOAT3& direction) const;
    DirectX::XMFLOAT3 getDirectionInWorldSpace3f(DirectX::FXMVECTOR direction) const;
    DirectX::XMFLOAT3 getDirectionInWorldSpace3f(const DirectX::XMFLOAT3& direction) const;

    void setVelocity(DirectX::FXMVECTOR velocity);
    void setVelocity3f(const DirectX::XMFLOAT3& velocity);
    void setVelocity(float x, float y, float z);
    DirectX::XMVECTOR getVelocity() const;
    const DirectX::XMFLOAT3& getVelocity3f() const;

    void addVelocity3f(const DirectX::XMFLOAT3& deltaVelocity);
    void addVelocity(DirectX::FXMVECTOR deltaVelocity);

    void setRotation3f(const DirectX::XMFLOAT3& rotation);
    void setRotation(DirectX::FXMVECTOR rotation);
    void setRotation(float x, float y, float z);

    DirectX::XMVECTOR getRotation() const;
    const DirectX::XMFLOAT3& getRotation3f() const;

    void addRotation(DirectX::XMVECTOR deltaRotation);
    void addRotation3f(const DirectX::XMFLOAT3& deltaRotation);

    bool getAwake() const;
    void setAwake(bool awake = true);

    bool getCanSleep() const;
    void setCanSleep(const bool canSleep = true);

    const DirectX::XMFLOAT3& getLastFrameAcceleration3f() const;
    DirectX::XMVECTOR getLastFrameAcceleration() const;

    void clearAccumulators();

    void addForce3f(const DirectX::XMFLOAT3& force);
    void addForce(DirectX::FXMVECTOR force);

    void addForceAtPoint3f(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& point);
    void addForceAtPoint(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point);
    void addForceAtPoint(const DirectX::XMFLOAT3& force, DirectX::FXMVECTOR point);
    void addForceAtPoint(DirectX::FXMVECTOR force, const DirectX::XMFLOAT3& point);

    void addForceAtBodyPoint3f(const DirectX::XMFLOAT3& force, const DirectX::XMFLOAT3& point);
    void addForceAtBodyPoint(DirectX::FXMVECTOR force, DirectX::FXMVECTOR point);
    void addForceAtBodyPoint(const DirectX::XMFLOAT3& force, DirectX::FXMVECTOR point);
    void addForceAtBodyPoint(DirectX::FXMVECTOR force, const DirectX::XMFLOAT3& point);

    void addTorque3f(const DirectX::XMFLOAT3& torque);
    void addTorque(DirectX::FXMVECTOR torque);

    void setAcceleration3f(const DirectX::XMFLOAT3& acceleration);
    void setAcceleration(DirectX::FXMVECTOR acceleration);
    void setAcceleration(float x, float y, float z);

    const DirectX::XMFLOAT3& getAcceleration3f() const;
    DirectX::XMVECTOR getAcceleration() const;
};