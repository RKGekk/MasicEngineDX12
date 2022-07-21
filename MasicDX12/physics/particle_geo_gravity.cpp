#include "particle_geo_gravity.h"

ParticleGeoGravity::ParticleGeoGravity(const DirectX::XMFLOAT3& center, float g) : m_center(center), m_g(g) {}

ParticleGeoGravity::ParticleGeoGravity(DirectX::FXMVECTOR center, float g) : m_g(g) {
	DirectX::XMStoreFloat3(&m_center, center);

}

void ParticleGeoGravity::updateForce(Particle* particle, float duration) {
	using namespace DirectX;
	if (!particle->hasFiniteMass()) { return; }

	DirectX::XMVECTOR center = DirectX::XMLoadFloat3(&m_center);
	DirectX::XMVECTOR pos = particle->getPosition();
	DirectX::XMVECTOR normal = XMVector3Normalize(center - pos);
	particle->addForce(m_g * normal * particle->getMass());
}
