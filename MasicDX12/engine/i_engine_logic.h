#pragma once

#include <memory>

#include <DirectXMath.h>

#include <pugixml/pugixml.hpp>

#include "../actors/actor.h"
#include "i_engine_physics.h"
#include "base_engine_state.h"

class IEngineLogic {
public:
	virtual WeakActorPtr VGetActor(const ActorId id) = 0;
	virtual StrongActorPtr VCreateActor(const std::string& actorResource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initialTransform, const ActorId serversActorId = INVALID_ACTOR_ID) = 0;
	virtual StrongActorPtr VCreateActor(const std::string& actorResource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* initialTransform, const ActorId serversActorId = INVALID_ACTOR_ID) = 0;
	virtual void VDestroyActor(const ActorId actorId) = 0;
	virtual bool VLoadGame(const char* levelResource) = 0;
	virtual void VOnUpdate(float time, float elapsedTime) = 0;
	virtual void VChangeState(enum BaseEngineState newState) = 0;
	virtual void VMoveActor(const ActorId id, DirectX::FXMMATRIX mat) = 0;
	virtual void VMoveActor4x4f(const ActorId id, const DirectX::XMFLOAT4X4& mat) = 0;
	virtual IEnginePhysics* VGetGamePhysics() = 0;
};