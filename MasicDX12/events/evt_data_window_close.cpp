#include "evt_data_window_close.h"

const std::string EvtData_Window_Close::sk_EventName = "EvtData_Window_Close";

EventTypeId EvtData_Window_Close::VGetEventType() const {
    return sk_EventType;
}

EvtData_Window_Close::EvtData_Window_Close() {
    m_confirm_close = true;
}

EvtData_Window_Close::EvtData_Window_Close(HWND hwnd, bool confirm_close) : m_confirm_close(confirm_close), m_hwnd(hwnd) {}

void EvtData_Window_Close::VSerialize(std::ostream& out) const {}

void EvtData_Window_Close::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Window_Close::VCopy() const {
    return IEventDataPtr(new EvtData_Window_Close(m_hwnd, m_confirm_close));
}

const std::string& EvtData_Window_Close::GetName() const {
    return sk_EventName;
}

bool EvtData_Window_Close::GetConfirmation() const {
    return m_confirm_close;
}

HWND EvtData_Window_Close::GetHWND() {
    return m_hwnd;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Window_Close& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event confirmation: " << evt.m_confirm_close << std::endl;
    os.flags(oldFlag);
    return os;
}