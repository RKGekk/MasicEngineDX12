#pragma once

#include <vector>
#include <memory>

#include <DirectXMath.h>

#include "../actors/actor.h"
#include "../physics/particle.h"
#include "../physics/particle_contact_generator.h"

class IEnginePhysics {
public:

	virtual bool VInitialize() = 0;
	virtual void VOnUpdate(float deltaSeconds) = 0;
	virtual void VSyncVisibleScene() = 0;

	virtual void VAddParticleActor(const ActorId actorId) = 0;
	virtual std::vector<Particle*>& VGetParticles() = 0;
	virtual void VRemoveActorParticle(ActorId id) = 0;
	virtual void VRemoveParticle(Particle* p) = 0;
	virtual ActorId VGetParticleActor(Particle* p) = 0;

	virtual void VAddContactGenerator(ActorId id) = 0;
	virtual void VRemoveContactGenerator(ActorId id) = 0;

	virtual void VAddForceGenerator(ActorId id) = 0;
	virtual void VRemoveForceGenerator(ActorId id) = 0;

	virtual ~IEnginePhysics() {};
};