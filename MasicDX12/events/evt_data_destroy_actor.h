#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"

class EvtData_Destroy_Actor : public BaseEventData {
    ActorId m_id;

public:
    static const EventTypeId sk_EventType = 0x77dd2b3a;
    static const std::string sk_EventName;

    explicit EvtData_Destroy_Actor(ActorId id = 0);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    ActorId GetId() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Destroy_Actor& evt);
};