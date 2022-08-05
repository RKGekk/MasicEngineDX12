#include "evt_data_mouse_button_released.h"

const std::string EvtData_Mouse_Button_Released::sk_EventName = "EvtData_Mouse_Button_Released";

EventTypeId EvtData_Mouse_Button_Released::VGetEventType() const {
    return sk_EventType;
}

EvtData_Mouse_Button_Released::EvtData_Mouse_Button_Released() {}

EvtData_Mouse_Button_Released::EvtData_Mouse_Button_Released(MBEventArgs e) : m_state(e) {}

EvtData_Mouse_Button_Released::EvtData_Mouse_Button_Released(MouseButtonSide button, MKState state, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y) : m_state(button, state, left_button, middle_button, right_button, control, shift, x, y) {}

void EvtData_Mouse_Button_Released::VSerialize(std::ostream& out) const {}

void EvtData_Mouse_Button_Released::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Mouse_Button_Released::VCopy() const {
    return IEventDataPtr(new EvtData_Mouse_Button_Released(m_state));
}

const std::string& EvtData_Mouse_Button_Released::GetName() const {
    return sk_EventName;
}

MouseButtonSide EvtData_Mouse_Button_Released::GetMouseButton() const {
    return m_state.Button;
}

MKState EvtData_Mouse_Button_Released::GetButtonState() const {
    return m_state.State;
}

bool EvtData_Mouse_Button_Released::GetLeftButton() const {
    return m_state.LeftButton;
}

bool EvtData_Mouse_Button_Released::GetMiddleButton() const {
    return m_state.MiddleButton;
}

bool EvtData_Mouse_Button_Released::GetRightButton() const {
    return m_state.RightButton;
}

bool EvtData_Mouse_Button_Released::GetControl() const {
    return m_state.Control;
}

bool EvtData_Mouse_Button_Released::GetShift() const {
    return m_state.Shift;
}

int EvtData_Mouse_Button_Released::GetX() const {
    return m_state.X;
}

int EvtData_Mouse_Button_Released::GetY() const {
    return m_state.Y;
}


std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Button_Released& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    switch (evt.m_state.Button) {
        case MouseButtonSide::None: os << "Event button pressed: none" << std::endl; break;
        case MouseButtonSide::Left: os << "Event button pressed: left" << std::endl; break;
        case MouseButtonSide::Right: os << "Event button pressed: right" << std::endl; break;
        case MouseButtonSide::Middle: os << "Event button pressed: middle" << std::endl; break;
        default: break;
    }
    os << "Event button state: " << (evt.m_state.State == MKState::Pressed ? "pressed" : "released") << std::endl;
    os << "Event left button pressed: " << evt.m_state.LeftButton << std::endl;
    os << "Event right button pressed: " << evt.m_state.RightButton << std::endl;
    os << "Event middle button pressed: " << evt.m_state.MiddleButton << std::endl;
    os << "Event control key pressed: " << evt.m_state.Control << std::endl;
    os << "Event shift key pressed: " << evt.m_state.Shift << std::endl;
    os << "Event x: " << evt.m_state.X << std::endl;
    os << "Event y: " << evt.m_state.Y << std::endl;
    os.flags(oldFlag);
    return os;
}