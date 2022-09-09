#pragma once

#include <map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#include <pugixml/pugixml.hpp>

#include "Actor.h"
#include "actor_component.h"
#include "../tools/generic_object_factory.h"
#include "../tools/memory_utility.h"

class ActorFactory {
    ActorId m_last_actorId;

protected:
    GenericObjectFactory<ActorComponent, ComponentId> m_component_factory;

public:
    ActorFactory();

    std::shared_ptr<Actor> CreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, const DirectX::XMFLOAT4X4* initial_transform, const ActorId servers_actorId);
    std::shared_ptr<Actor> CreateActor(const std::string& actor_resource, const pugi::xml_node& overrides, DirectX::FXMMATRIX initial_transform, const ActorId servers_actorId);
    void ModifyActor(std::shared_ptr<Actor> pActor, const pugi::xml_node& overrides);

    virtual std::shared_ptr<ActorComponent> VCreateComponent(const pugi::xml_node& pData);

private:
    ActorId GetNextActorId();
};