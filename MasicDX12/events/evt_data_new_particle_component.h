#pragma once

#include <string>
#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"
#include "../physics/particle.h"

class SceneNode;

class EvtData_New_Particle_Component : public BaseEventData {
    ActorId m_actorId;
    Particle* m_pParticle;

public:
    static const EventTypeId sk_EventType = 0x4e170c71;
    static const std::string sk_EventName;

    EvtData_New_Particle_Component();
    explicit EvtData_New_Particle_Component(ActorId actorId, Particle* pParticle);

    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual EventTypeId VGetEventType() const override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    ActorId GetActorId() const;
    Particle* GetParticlePtr();

    friend std::ostream& operator<<(std::ostream& os, const EvtData_New_Particle_Component& evt);
};