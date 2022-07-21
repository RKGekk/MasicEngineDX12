#pragma once

#include <Windows.h>

#include <iostream>
#include <iomanip>

#include "base_event_data.h"
#include "../engine/os_message_data.h"

class EvtData_OS_Message : public BaseEventData {
	OSMessageData m_data;
	unsigned int m_viewId;

public:
	static const EventTypeId sk_EventType = 0xd158c6ee;
	static const std::string sk_EventName;

	EvtData_OS_Message();
	explicit EvtData_OS_Message(OSMessageData data);

	virtual void VDeserialize(std::istream& in) override;
	virtual EventTypeId VGetEventType() const override;
	virtual IEventDataPtr VCopy() const override;
	virtual void VSerialize(std::ostream& out) const override;
	virtual const std::string& GetName() const override;

	const OSMessageData GetOSMessage() const;

	friend std::ostream& operator<<(std::ostream& os, const EvtData_OS_Message& evt);
};