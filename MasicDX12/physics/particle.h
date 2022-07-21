#pragma once

#include <DirectXMath.h>

#include "../tools/math_utitity.h"

class Particle {
protected:
    float m_inverse_mass;
    float m_damping;
    float m_radius;
    float m_ttl;
    float m_time_accum;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_force_accum;
    DirectX::XMFLOAT3 m_acceleration;

    bool m_is_awake;
    bool m_can_sleep;
    const float m_sleep_epsilon = 0.8f;
    float m_motion;

public:
    void integrate(float duration);

    void setMass(float mass);
    float getMass() const;

    bool getCanSleep() const;
    void setCanSleep(bool canSleep = true);
    void setAwake(bool awake = true);
    bool getAwake() const;

    void setRadius(float radius);
    float getRadius() const;

    void setTTL(float ttl);
    float getTTL() const;
    float getTTLLast() const;
    float getTTLNormal() const;
    void clearTimeAccumulator();

    void setInverseMass(float inverseMass);
    float getInverseMass() const;
    bool hasFiniteMass() const;

    void setDamping(float damping);
    float getDamping() const;

    void setPosition3f(const DirectX::XMFLOAT3& position);
    void setPosition(DirectX::FXMVECTOR position);
    void setPosition(float x, float y, float z);
    const DirectX::XMFLOAT3& getPosition3f() const;
    DirectX::XMVECTOR getPosition() const;

    void setVelocity3f(const DirectX::XMFLOAT3& velocity);
    void setVelocity(DirectX::FXMVECTOR velocity);
    void setVelocity(float x, float y, float z);
    const DirectX::XMFLOAT3& getVelocity3f() const;
    DirectX::XMVECTOR getVelocity() const;

    void setAcceleration3f(const DirectX::XMFLOAT3& acceleration);
    void setAcceleration(DirectX::FXMVECTOR acceleration);
    void setAcceleration(float x, float y, float z);
    const DirectX::XMFLOAT3& getAcceleration3f() const;
    DirectX::XMVECTOR getAcceleration() const;

    void clearAccumulator();
    void addForce3f(const DirectX::XMFLOAT3& force);
    void addForce(const DirectX::FXMVECTOR force);
};