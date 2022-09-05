#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"

class EvtData_Environment_Loaded : public BaseEventData {
public:
	static const EventTypeId sk_EventType = 0x8E2AD6E6;
	static const std::string sk_EventName;

	EvtData_Environment_Loaded();

	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual const std::string& GetName() const override;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Environment_Loaded& evt);
};