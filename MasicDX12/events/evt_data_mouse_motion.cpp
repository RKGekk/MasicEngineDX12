#include "evt_data_mouse_motion.h"

const std::string EvtData_Mouse_Motion::sk_EventName = "EvtData_Mouse_Motion";

EventTypeId EvtData_Mouse_Motion::VGetEventType() const {
    return sk_EventType;
}

EvtData_Mouse_Motion::EvtData_Mouse_Motion() {}

EvtData_Mouse_Motion::EvtData_Mouse_Motion(MouseMotionEventArgs e) : m_state(e) {}

EvtData_Mouse_Motion::EvtData_Mouse_Motion(bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y, int rel_x, int rel_y) : m_state(left_button, middle_button, right_button, control, shift, x, y, rel_x, rel_y) {}

void EvtData_Mouse_Motion::VSerialize(std::ostream& out) const {}

void EvtData_Mouse_Motion::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Mouse_Motion::VCopy() const {
    return IEventDataPtr(new EvtData_Mouse_Motion(m_state));
}

const std::string& EvtData_Mouse_Motion::GetName() const {
    return sk_EventName;
}

bool EvtData_Mouse_Motion::GetLeftButton() const {
    return m_state.LeftButton;
}

bool EvtData_Mouse_Motion::GetMiddleButton() const {
    return m_state.MiddleButton;
}

bool EvtData_Mouse_Motion::GetRightButton() const {
    return m_state.RightButton;
}

bool EvtData_Mouse_Motion::GetControl() const {
    return m_state.Control;
}

bool EvtData_Mouse_Motion::GetShift() const {
    return m_state.Shift;
}

int EvtData_Mouse_Motion::GetX() const {
    return m_state.X;
}

int EvtData_Mouse_Motion::GetY() const {
    return m_state.Y;
}

int EvtData_Mouse_Motion::GetRelX() const {
    return m_state.RelX;
}

int EvtData_Mouse_Motion::GetRelY() const {
    return m_state.RelY;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Motion& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event left button pressed: " << evt.m_state.LeftButton << std::endl;
    os << "Event right button pressed: " << evt.m_state.RightButton << std::endl;
    os << "Event middle button pressed: " << evt.m_state.MiddleButton << std::endl;
    os << "Event control key pressed: " << evt.m_state.Control << std::endl;
    os << "Event shift key pressed: " << evt.m_state.Shift << std::endl;
    os << "Event x: " << evt.m_state.X << std::endl;
    os << "Event y: " << evt.m_state.Y << std::endl;
    os << "Event x since the last event: " << evt.m_state.X << std::endl;
    os << "Event y since the last event: " << evt.m_state.RelY << std::endl;
    os.flags(oldFlag);
    return os;
}