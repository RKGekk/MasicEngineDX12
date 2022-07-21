#include "evt_data_modified_render_component.h"

const std::string EvtData_Modified_Render_Component::sk_EventName = "EvtData_Modified_Render_Component";

EventTypeId EvtData_Modified_Render_Component::VGetEventType() const {
    return sk_EventType;
}

EvtData_Modified_Render_Component::EvtData_Modified_Render_Component() {
    m_id = 0;
    m_componentId = 0;
}

EvtData_Modified_Render_Component::EvtData_Modified_Render_Component(ActorId id, ComponentId cId) {
    m_id = id;
    m_componentId = cId;
}

void EvtData_Modified_Render_Component::VSerialize(std::ostream& out) const {
    out << m_id;
}

void EvtData_Modified_Render_Component::VDeserialize(std::istream& in) {
    in >> m_id;
}

IEventDataPtr EvtData_Modified_Render_Component::VCopy() const {
    return IEventDataPtr(new EvtData_Modified_Render_Component(m_id, m_componentId));
}

const std::string& EvtData_Modified_Render_Component::GetName() const {
    return sk_EventName;
}

ActorId EvtData_Modified_Render_Component::GetActorId() const {
    return m_id;
}

ComponentId EvtData_Modified_Render_Component::GetComponentId() const {
    return m_componentId;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Modified_Render_Component& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event actor id: " << evt.m_id << std::endl;
    os.flags(oldFlag);
    return os;
}