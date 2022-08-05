#pragma once

#include <iostream>
#include <iomanip>

#include "base_event_data.h"

class EvtData_Update_Tick : public BaseEventData, public GameTimerDelta {
public:
	static const EventTypeId sk_EventType = 0xf0f5d183;
	static const std::string sk_EventName;

	EvtData_Update_Tick();
	explicit EvtData_Update_Tick(GameClockDuration delta_time, GameClockDuration total_time);
	explicit EvtData_Update_Tick(GameTimerDelta delta_time);

	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_Update_Tick& evt);
};