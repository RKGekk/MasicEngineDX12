#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "..//actors/actor.h"

class EvtData_New_Actor : public BaseEventData {
	ActorId m_actorId;
	unsigned int m_viewId;

public:
	static const EventTypeId sk_EventType = 0xe86c7c31;
	static const std::string sk_EventName;

	EvtData_New_Actor();
	explicit EvtData_New_Actor(ActorId actorId, unsigned long viewId = 0xffffffff);

	virtual void VDeserialize(std::istream& in) override;
	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	const ActorId GetActorId() const;
	unsigned int GetViewId() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_New_Actor& evt);
};