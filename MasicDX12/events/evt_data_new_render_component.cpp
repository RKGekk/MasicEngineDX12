#include "evt_data_new_render_component.h"

const std::string EvtData_New_Render_Component::sk_EventName = "EvtData_New_Render_Component";

EvtData_New_Render_Component::EvtData_New_Render_Component() {}

EvtData_New_Render_Component::EvtData_New_Render_Component(ActorId actorId, ComponentId componentId, std::shared_ptr<SceneNode> pSceneNode) {
    m_componentId = componentId;
    m_actorId = actorId;
    m_pSceneNode = pSceneNode;
}

void EvtData_New_Render_Component::VSerialize(std::ostream& out) const {}
void EvtData_New_Render_Component::VDeserialize(std::istream& in) {}

EventTypeId EvtData_New_Render_Component::VGetEventType() const {
    return sk_EventType;
}

IEventDataPtr EvtData_New_Render_Component::VCopy() const {
    return IEventDataPtr(new EvtData_New_Render_Component(m_actorId, m_componentId, m_pSceneNode));
}

const std::string& EvtData_New_Render_Component::GetName() const {
    return sk_EventName;
}

ActorId EvtData_New_Render_Component::GetActorId() const {
    return m_actorId;
}

ComponentId EvtData_New_Render_Component::GetComponentId() const {
    return m_componentId;
}

std::shared_ptr<SceneNode> EvtData_New_Render_Component::GetSceneNode() const {
    return m_pSceneNode;
}

std::ostream& operator<<(std::ostream& os, const EvtData_New_Render_Component& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event actor id: " << evt.m_actorId << std::endl;
    os.flags(oldFlag);
    return os;
}