#include "evt_data_request_destroy_actor.h"

const std::string EvtData_Request_Destroy_Actor::sk_EventName = "EvtData_Request_Destroy_Actor";

EvtData_Request_Destroy_Actor::EvtData_Request_Destroy_Actor() {
	m_actorId = 0;
}

EvtData_Request_Destroy_Actor::EvtData_Request_Destroy_Actor(ActorId actorId) {
	m_actorId = actorId;
}

void EvtData_Request_Destroy_Actor::VDeserialize(std::istream& in) {
	in >> m_actorId;
}

EventTypeId EvtData_Request_Destroy_Actor::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_Request_Destroy_Actor::VCopy() const {
	return IEventDataPtr(new EvtData_Request_Destroy_Actor(m_actorId));
}

void EvtData_Request_Destroy_Actor::VSerialize(std::ostream& out) const {
	out << m_actorId << " ";
}

const std::string& EvtData_Request_Destroy_Actor::GetName() const {
	return sk_EventName;
}

const ActorId EvtData_Request_Destroy_Actor::GetActorId() const {
	return m_actorId;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Request_Destroy_Actor& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event actor id: " << evt.m_actorId << std::endl;
	os.flags(oldFlag);
	return os;
}