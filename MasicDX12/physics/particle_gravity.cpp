#include "particle_gravity.h"

ParticleGravity::ParticleGravity(const DirectX::XMFLOAT3& gravity) {
	m_gravity = gravity;
}

ParticleGravity::ParticleGravity(DirectX::FXMVECTOR gravity) {
	DirectX::XMStoreFloat3(&m_gravity, gravity);
}

void ParticleGravity::updateForce(Particle* particle, float duration) {
	using namespace DirectX;
	if (!particle->hasFiniteMass()) { return; }

	DirectX::XMVECTOR gravity = DirectX::XMLoadFloat3(&m_gravity);
	particle->addForce(gravity * particle->getMass());
}
