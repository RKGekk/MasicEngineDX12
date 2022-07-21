#include "evt_data_move_actor.h"

const std::string EvtData_Move_Actor::sk_EventName = "EvtData_Move_Actor";

EventTypeId EvtData_Move_Actor::VGetEventType() const {
    return sk_EventType;
}

EvtData_Move_Actor::EvtData_Move_Actor() {
    m_id = 0;
}

EvtData_Move_Actor::EvtData_Move_Actor(ActorId id, const DirectX::XMFLOAT4X4& matrix) {
    m_id = id;
    m_matrix = matrix;
}

void EvtData_Move_Actor::VSerialize(std::ostream& out) const {
    out << m_id << " ";
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            out << m_matrix.m[i][j] << " ";
        }
    }
}

void EvtData_Move_Actor::VDeserialize(std::istream& in) {
    in >> m_id;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            in >> m_matrix.m[i][j];
        }
    }
}

IEventDataPtr EvtData_Move_Actor::VCopy() const {
    return IEventDataPtr(new EvtData_Move_Actor(m_id, m_matrix));
}

const std::string& EvtData_Move_Actor::GetName() const {
    return sk_EventName;
}

ActorId EvtData_Move_Actor::GetId() const {
    return m_id;
}

const DirectX::XMFLOAT4X4& EvtData_Move_Actor::GetMatrix4x4() const {
    return m_matrix;
}

DirectX::XMMATRIX EvtData_Move_Actor::GetMatrix() const {
    return DirectX::XMLoadFloat4x4(&m_matrix);
}

std::ostream& operator<<(std::ostream& os, const EvtData_Move_Actor& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event actor id: " << evt.m_id << std::endl;
    os << "Event matrix: " << std::endl << evt.m_matrix << std::endl;
    os.flags(oldFlag);
    return os;
}