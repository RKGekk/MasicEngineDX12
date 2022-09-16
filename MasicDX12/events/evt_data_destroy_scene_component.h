#pragma once

#include <string>
#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"

class SceneNode;

class EvtData_Destroy_Scene_Component : public BaseEventData {
    ActorId m_actorId = INVALID_ACTOR_ID;
    ComponentId m_componentId = INVALID_COMPONENT_ID;
    std::weak_ptr<SceneNode> m_pSceneNode;

public:
    static const EventTypeId sk_EventType = 0xd7a57113;
    static const std::string sk_EventName;

    EvtData_Destroy_Scene_Component();
    explicit EvtData_Destroy_Scene_Component(ActorId actorId, ComponentId componentId, std::weak_ptr<SceneNode> pSceneNode);

    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual EventTypeId VGetEventType() const override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    ActorId GetActorId() const;
    ComponentId GetComponentId() const;
    std::weak_ptr<SceneNode> GetSceneNode() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Destroy_Scene_Component& evt);
};