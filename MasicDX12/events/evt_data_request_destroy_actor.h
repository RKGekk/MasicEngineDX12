#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"

class EvtData_Request_Destroy_Actor : public BaseEventData {
	ActorId m_actorId;

public:
	static const EventTypeId sk_EventType = 0xf5395770;
	static const std::string sk_EventName;

	EvtData_Request_Destroy_Actor();
	explicit EvtData_Request_Destroy_Actor(ActorId actorId);

	virtual void VDeserialize(std::istream& in) override;
	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	const ActorId GetActorId() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Request_Destroy_Actor& evt);
};