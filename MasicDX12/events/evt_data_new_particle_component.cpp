#include "evt_data_new_particle_component.h"

const std::string EvtData_New_Particle_Component::sk_EventName = "EvtData_New_Particle_Component";

EvtData_New_Particle_Component::EvtData_New_Particle_Component() {
    m_actorId = 0;
    m_pParticle = nullptr;
}

EvtData_New_Particle_Component::EvtData_New_Particle_Component(ActorId actorId, Particle* pParticle) {
    m_actorId = actorId;
    m_pParticle = pParticle;
}

void EvtData_New_Particle_Component::VSerialize(std::ostream& out) const {}
void EvtData_New_Particle_Component::VDeserialize(std::istream& in) {}

EventTypeId EvtData_New_Particle_Component::VGetEventType() const {
    return sk_EventType;
}

IEventDataPtr EvtData_New_Particle_Component::VCopy() const {
    return IEventDataPtr(new EvtData_New_Particle_Component(m_actorId, m_pParticle));
}

const std::string& EvtData_New_Particle_Component::GetName() const {
    return sk_EventName;
}

ActorId EvtData_New_Particle_Component::GetActorId() const {
    return m_actorId;
}

Particle* EvtData_New_Particle_Component::GetParticlePtr() {
    return m_pParticle;
}

std::ostream& operator<<(std::ostream& os, const EvtData_New_Particle_Component& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event actor id: " << evt.GetActorId() << std::endl;
    os.flags(oldFlag);
    return os;
}