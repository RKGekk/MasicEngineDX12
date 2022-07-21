#include "particle.h"
#include <cmath>

#include "../tools/math_utitity.h"

void Particle::integrate(float duration) {
    using namespace DirectX;

    if (!m_is_awake) { return; }

    m_time_accum += duration;
    if (m_time_accum > m_ttl) { m_time_accum = m_ttl; }

    if (m_inverse_mass <= EPSILON) return;

    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_position);
    DirectX::XMVECTOR vel = DirectX::XMLoadFloat3(&m_velocity);
    DirectX::XMStoreFloat3(&m_position, pos + vel * duration);

    DirectX::XMVECTOR resultingAcc = DirectX::XMLoadFloat3(&m_acceleration);
    DirectX::XMVECTOR forceAccum = DirectX::XMLoadFloat3(&m_force_accum);
    resultingAcc += forceAccum * m_inverse_mass;
    vel += resultingAcc * duration;

    float dump = std::powf(m_damping, duration);
    vel = DirectX::XMVectorScale(vel, dump);
    DirectX::XMStoreFloat3(&m_velocity, vel);

    clearAccumulator();

    if (m_can_sleep) {
        float currentMotion = XMVectorGetX(XMVector3Dot(vel, vel));

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

void Particle::setMass(float mass) {
    m_inverse_mass = 1.0f / mass;
}

float Particle::getMass() const {
    if (m_inverse_mass <= EPSILON) {
        return std::numeric_limits<float>::max();
    }
    else {
        return 1.0f / m_inverse_mass;
    }
}

bool Particle::getCanSleep() const {
    return m_can_sleep;
}

void Particle::setCanSleep(const bool canSleep) {
    m_can_sleep = canSleep;
    if (!canSleep && !m_is_awake) {
        setAwake();
    }
}

void Particle::setAwake(bool awake) {
    if (awake) {
        m_is_awake = true;
        m_motion = m_sleep_epsilon * 2.0f;
    }
    else {
        m_is_awake = false;
        m_velocity.x = 0.0f; m_velocity.y = 0.0f; m_velocity.z = 0.0f;
    }
}

bool Particle::getAwake() const {
    return m_is_awake;
}

void Particle::setRadius(float radius) {
    m_radius = radius;
}

float Particle::getRadius() const {
    return m_radius;
}

void Particle::setTTL(float ttl) {
    m_ttl = ttl;
}

float Particle::getTTL() const {
    return m_ttl;
}

float Particle::getTTLLast() const {
    float time_diff = m_ttl - m_time_accum;
    return std::fmaxf(0.0f, time_diff);
}

float Particle::getTTLNormal() const {
    if (m_ttl > 0.0f) {
        float last = getTTLLast();
        float res = last / m_ttl;
        return res;
    }
    return 1.0f;
}

void Particle::clearTimeAccumulator() {
    m_time_accum = 0.0f;
}

void Particle::setInverseMass(float inverseMass) {
    m_inverse_mass = inverseMass;
}

float Particle::getInverseMass() const {
    return m_inverse_mass;
}

bool Particle::hasFiniteMass() const {
    return m_inverse_mass >= EPSILON;
}

void Particle::setDamping(float damping) {
    m_damping = damping;
}

float Particle::getDamping() const {
    return m_damping;
}

void Particle::setPosition3f(const DirectX::XMFLOAT3& position) {
    m_position = position;
}

void Particle::setPosition(DirectX::FXMVECTOR position) {
    DirectX::XMStoreFloat3(&m_position, position);
}

void Particle::setPosition(float x, float y, float z) {
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

const DirectX::XMFLOAT3& Particle::getPosition3f() const {
    return m_position;
}

DirectX::XMVECTOR Particle::getPosition() const {
    return DirectX::XMLoadFloat3(&m_position);
}

void Particle::setVelocity3f(const DirectX::XMFLOAT3& velocity) {
    m_velocity = velocity;
}

void Particle::setVelocity(DirectX::FXMVECTOR velocity) {
    DirectX::XMStoreFloat3(&m_velocity, velocity);
}

void Particle::setVelocity(float x, float y, float z) {
    m_velocity.x = x;
    m_velocity.y = y;
    m_velocity.z = z;
}

const DirectX::XMFLOAT3& Particle::getVelocity3f() const {
    return m_velocity;
}

DirectX::XMVECTOR Particle::getVelocity() const {
    return DirectX::XMLoadFloat3(&m_velocity);
}

void Particle::setAcceleration3f(const DirectX::XMFLOAT3& acceleration) {
    m_acceleration = acceleration;
}

void Particle::setAcceleration(DirectX::FXMVECTOR acceleration) {
    DirectX::XMStoreFloat3(&m_acceleration, acceleration);
}

void Particle::setAcceleration(float x, float y, float z) {
    m_acceleration.x = x;
    m_acceleration.y = y;
    m_acceleration.z = z;
}

const DirectX::XMFLOAT3& Particle::getAcceleration3f() const {
    return m_acceleration;
}

DirectX::XMVECTOR Particle::getAcceleration() const {
    return DirectX::XMLoadFloat3(&m_acceleration);
}

void Particle::clearAccumulator() {
    m_force_accum.x = 0.0f;
    m_force_accum.y = 0.0f;
    m_force_accum.z = 0.0f;
}

void Particle::addForce3f(const DirectX::XMFLOAT3& force) {
    using namespace DirectX;
    XMVECTOR fn = XMLoadFloat3(&force);
    XMVECTOR fo = XMLoadFloat3(&m_force_accum);
    XMStoreFloat3(&m_force_accum, fo + fn);
}

void Particle::addForce(const DirectX::FXMVECTOR fn) {
    using namespace DirectX;
    XMVECTOR fo = XMLoadFloat3(&m_force_accum);
    XMStoreFloat3(&m_force_accum, fo + fn);
}
