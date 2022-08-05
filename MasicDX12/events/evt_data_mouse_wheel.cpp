#include "evt_data_mouse_wheel.h"

const std::string EvtData_Mouse_Wheel::sk_EventName = "EvtData_Mouse_Wheel";

EventTypeId EvtData_Mouse_Wheel::VGetEventType() const {
    return sk_EventType;
}

EvtData_Mouse_Wheel::EvtData_Mouse_Wheel(MouseWheelEventArgs e) : m_stat(e) {}

EvtData_Mouse_Wheel::EvtData_Mouse_Wheel() {}

EvtData_Mouse_Wheel::EvtData_Mouse_Wheel(float wheel_delta, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y) : m_stat(wheel_delta, left_button, middle_button, right_button, control, shift, x, y) {}

void EvtData_Mouse_Wheel::VSerialize(std::ostream& out) const {}

void EvtData_Mouse_Wheel::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Mouse_Wheel::VCopy() const {
    return IEventDataPtr(new EvtData_Mouse_Wheel(m_stat));
}

const std::string& EvtData_Mouse_Wheel::GetName() const {
    return sk_EventName;
}

float EvtData_Mouse_Wheel::GetWheelDelta() const {
    return m_stat.WheelDelta;
}

bool EvtData_Mouse_Wheel::GetLeftButton() const {
    return m_stat.LeftButton;
}

bool EvtData_Mouse_Wheel::GetMiddleButton() const {
    return m_stat.MiddleButton;
}

bool EvtData_Mouse_Wheel::GetRightButton() const {
    return m_stat.RightButton;
}

bool EvtData_Mouse_Wheel::GetControl() const {
    return m_stat.Control;
}

bool EvtData_Mouse_Wheel::GetShift() const {
    return m_stat.Shift;
}

int EvtData_Mouse_Wheel::GetX() const {
    return m_stat.X;
}

int EvtData_Mouse_Wheel::GetY() const {
    return m_stat.Y;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Wheel& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event wheel delta: " << evt.m_stat.WheelDelta << std::endl;
    os << "Event left button pressed: " << evt.m_stat.LeftButton << std::endl;
    os << "Event right button pressed: " << evt.m_stat.RightButton << std::endl;
    os << "Event middle button pressed: " << evt.m_stat.MiddleButton << std::endl;
    os << "Event control key pressed: " << evt.m_stat.Control << std::endl;
    os << "Event shift key pressed: " << evt.m_stat.Shift << std::endl;
    os << "Event x: " << evt.m_stat.X << std::endl;
    os << "Event y: " << evt.m_stat.Y << std::endl;
    os.flags(oldFlag);
    return os;
}