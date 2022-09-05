#include "evt_data_sphere_particle_contact.h"

const std::string EvtData_Sphere_Particle_Contact::sk_EventName = "EvtData_Sphere_Particle_Contact";

EvtData_Sphere_Particle_Contact::EvtData_Sphere_Particle_Contact() {
	m_actorId_1 = 0;
	m_actorId_2 = 0;
}

EvtData_Sphere_Particle_Contact::EvtData_Sphere_Particle_Contact(ActorId actorId_1, ActorId actorId_2) {
	m_actorId_1 = actorId_1;
	m_actorId_2 = actorId_2;
}

void EvtData_Sphere_Particle_Contact::VDeserialize(std::istream& in) {
	in >> m_actorId_1;
	in >> m_actorId_2;
}

EventTypeId EvtData_Sphere_Particle_Contact::VGetEventType() const {
	return sk_EventType;
}

IEventDataPtr EvtData_Sphere_Particle_Contact::VCopy() const {
	return IEventDataPtr(new EvtData_Sphere_Particle_Contact(m_actorId_1, m_actorId_2));
}

void EvtData_Sphere_Particle_Contact::VSerialize(std::ostream& out) const {
	out << m_actorId_1 << " ";
	out << m_actorId_2 << " ";
}

const std::string& EvtData_Sphere_Particle_Contact::GetName() const {
	return sk_EventName;
}

const ActorId EvtData_Sphere_Particle_Contact::GetActorId1() const {
	return m_actorId_1;
}

const ActorId EvtData_Sphere_Particle_Contact::GetActorId2() const {
	return m_actorId_2;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Sphere_Particle_Contact& evt) {
	std::ios::fmtflags oldFlag = os.flags();
	os << "Event type id: " << evt.sk_EventType << std::endl;
	os << "Event name: " << evt.sk_EventName << std::endl;
	os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
	os << "Event actor1 id: " << evt.m_actorId_1 << std::endl;
	os << "Event actor2 id: " << evt.m_actorId_2 << std::endl;
	os.flags(oldFlag);
	return os;
}