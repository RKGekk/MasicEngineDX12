#pragma once

#include <string>
#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"

class SceneNode;

class EvtData_New_Render_Component : public BaseEventData {
    ActorId m_actorId = INVALID_ACTOR_ID;
    ComponentId m_componentId = INVALID_COMPONENT_ID;
    std::shared_ptr<SceneNode> m_pSceneNode;

public:
    static const EventTypeId sk_EventType = 0xaf4aff75;
    static const std::string sk_EventName;

    EvtData_New_Render_Component();
    explicit EvtData_New_Render_Component(ActorId actorId, ComponentId componentId, std::shared_ptr<SceneNode> pSceneNode);

    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual EventTypeId VGetEventType() const override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    ActorId GetActorId() const;
    ComponentId GetComponentId() const;
    std::shared_ptr<SceneNode> GetSceneNode() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_New_Render_Component& evt);
};