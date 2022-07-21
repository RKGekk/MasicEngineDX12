#include "evt_data_destroy_actor.h"

const std::string EvtData_Destroy_Actor::sk_EventName = "EvtData_Destroy_Actor";

EvtData_Destroy_Actor::EvtData_Destroy_Actor(ActorId id) {
	m_id = id;
}

EventTypeId EvtData_Destroy_Actor::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_Destroy_Actor::VCopy() const {
	return IEventDataPtr(new EvtData_Destroy_Actor(m_id));
}

void EvtData_Destroy_Actor::VSerialize(std::ostream& out) const {
	out << m_id;
}

void EvtData_Destroy_Actor::VDeserialize(std::istream& in) {
	in >> m_id;
}

const std::string& EvtData_Destroy_Actor::GetName() const {
	return sk_EventName;
}

ActorId EvtData_Destroy_Actor::GetId() const {
	return m_id;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Destroy_Actor& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event actor id: " << evt.m_id << std::endl;
	os.flags(oldFlag);
	return os;
}