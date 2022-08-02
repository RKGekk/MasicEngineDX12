#include "evt_data_dpi_scale.h"

const std::string EvtData_DPI_Scale::sk_EventName = "EvtData_DPI_Scale";

EventTypeId EvtData_DPI_Scale::VGetEventType() const {
    return sk_EventType;
}

EvtData_DPI_Scale::EvtData_DPI_Scale() {
    m_dpi_scale = 1.0f;
}

EvtData_DPI_Scale::EvtData_DPI_Scale(float dpi_scale) : m_dpi_scale(dpi_scale) {}

void EvtData_DPI_Scale::VSerialize(std::ostream& out) const {}

void EvtData_DPI_Scale::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_DPI_Scale::VCopy() const {
    return IEventDataPtr(new EvtData_DPI_Scale(m_dpi_scale));
}

const std::string& EvtData_DPI_Scale::GetName() const {
    return sk_EventName;
}

float EvtData_DPI_Scale::GetDPIScale() const {
    return m_dpi_scale;
}

std::ostream& operator<<(std::ostream& os, const EvtData_DPI_Scale& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event dpi scale: " << evt.m_dpi_scale << std::endl;
    os.flags(oldFlag);
    return os;
}