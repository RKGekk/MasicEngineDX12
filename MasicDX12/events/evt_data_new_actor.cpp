#include "evt_data_new_actor.h"

const std::string EvtData_New_Actor::sk_EventName = "EvtData_New_Actor";

EvtData_New_Actor::EvtData_New_Actor() {
	m_actorId = 0;
	m_viewId = 0;
}

EvtData_New_Actor::EvtData_New_Actor(ActorId actorId, unsigned long viewId) {
	m_actorId = actorId;
	m_viewId = viewId;
}

void EvtData_New_Actor::VDeserialize(std::istream& in) {
	in >> m_actorId;
	in >> m_viewId;
}

EventTypeId EvtData_New_Actor::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_New_Actor::VCopy() const {
	return IEventDataPtr(new EvtData_New_Actor(m_actorId, m_viewId));
}

void EvtData_New_Actor::VSerialize(std::ostream& out) const {
	out << m_actorId << " ";
	out << m_viewId << " ";
}

const std::string& EvtData_New_Actor::GetName() const {
	return sk_EventName;
}

const ActorId EvtData_New_Actor::GetActorId() const {
	return m_actorId;
}

unsigned int EvtData_New_Actor::GetViewId() const {
	return m_viewId;
}

std::ostream& operator<<(std::ostream& os, const EvtData_New_Actor& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event actor id: " << evt.m_actorId << std::endl;
	os << "Event view id: " << evt.m_viewId << std::endl;
	os.flags(oldFlag);
	return os;
}