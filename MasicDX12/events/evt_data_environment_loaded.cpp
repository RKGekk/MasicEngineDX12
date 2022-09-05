#include "evt_data_environment_loaded.h"

const std::string EvtData_Environment_Loaded::sk_EventName = "EvtData_Environment_Loaded";

EvtData_Environment_Loaded::EvtData_Environment_Loaded() {}

EventTypeId EvtData_Environment_Loaded::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_Environment_Loaded::VCopy() const {
	return IEventDataPtr(new EvtData_Environment_Loaded());
}

const std::string& EvtData_Environment_Loaded::GetName() const {
	return sk_EventName;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Environment_Loaded& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os.flags(oldFlag);
	return os;
}