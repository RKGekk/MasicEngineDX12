#pragma once

#include <vector>

#include "particle.h"
#include "particle_contact_generator.h"
#include "particle_world.h"

class ParticleSphereContact : public ParticleContactGenerator {
	ParticleWorld::Particles* m_particles;

public:
	void init(ParticleWorld::Particles* particles);

	virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override;
};