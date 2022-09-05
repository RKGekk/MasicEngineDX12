#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../actors/actor.h"
#include "../tools/string_utility.h"

class EvtData_Request_Start_Game : public BaseEventData {

public:
	static const EventTypeId sk_EventType = 0x11f2b19d;
	static const std::string sk_EventName;

	EvtData_Request_Start_Game();

	virtual EventTypeId VGetEventType() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual void VDeserialize(std::istream& in) override;
	virtual IEventDataPtr VCopy() const override;
	virtual const std::string& GetName() const override;
};