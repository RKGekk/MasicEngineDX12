#include "particle_fake_spring.h"
#include "../tools/math_utitity.h"

ParticleFakeSpring::ParticleFakeSpring(DirectX::XMFLOAT3* anchor, float springConstant, float damping) : m_anchor(anchor), m_springConstant(springConstant), m_damping(damping) {}

void ParticleFakeSpring::updateForce(Particle* particle, float duration) {
    using namespace DirectX;

    if (!particle->hasFiniteMass()) { return; }

    XMVECTOR position = particle->getPosition();
    position -= XMLoadFloat3(m_anchor);

    float gamma = 0.5f * XMVectorGetX(XMVectorSqrt(XMVectorSet(4.0f * m_springConstant - m_damping * m_damping, 0.0f, 0.0f, 0.0f)));
    if (XMScalarNearEqual(gamma, 0.0f, EPSILON)) { return; };
    XMVECTOR c = position * (m_damping / (2.0f * gamma)) + particle->getVelocity() * (1.0f / gamma);
    XMVECTOR target = position * XMScalarCos(gamma * duration) + c * XMScalarSin(gamma * duration);
    target *= XMVectorGetX(XMVectorExp(XMVectorSet(-0.5f * duration * m_damping, 0.0f, 0.0f, 0.0f)));

    XMVECTOR accel = (target - position) * (1.0f / (duration * duration)) - particle->getVelocity() * (1.0f / duration);
    particle->addForce(accel * particle->getMass());
}
