#include "evt_data_key_pressed_event.h"

#include "../tools/memory_utility.h"

const std::string EvtData_Key_Pressed_Event::sk_EventName = "EvtData_Key_Pressed_Event";

EventTypeId EvtData_Key_Pressed_Event::VGetEventType() const {
    return sk_EventType;
}

EvtData_Key_Pressed_Event::EvtData_Key_Pressed_Event() {}

EvtData_Key_Pressed_Event::EvtData_Key_Pressed_Event(KeyEventArgs e) : m_state(e) {}

EvtData_Key_Pressed_Event::EvtData_Key_Pressed_Event(WindowKey key, unsigned int c, KeyState state, bool control, bool shift, bool alt) : m_state(key, c, state, control, shift, alt) {}

void EvtData_Key_Pressed_Event::VSerialize(std::ostream& out) const {}

void EvtData_Key_Pressed_Event::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Key_Pressed_Event::VCopy() const {
    return IEventDataPtr(new EvtData_Key_Pressed_Event(m_state));
}

const std::string& EvtData_Key_Pressed_Event::GetName() const {
    return sk_EventName;
}

WindowKey EvtData_Key_Pressed_Event::GetWindowKeyCode() const {
    return m_state.Key;
}

unsigned int EvtData_Key_Pressed_Event::GetCharCode() const {
    return m_state.Char;
}

KeyState EvtData_Key_Pressed_Event::GetKeyPressState() const {
    return m_state.State;
}

bool EvtData_Key_Pressed_Event::GetControlButtonPressed() const {
    return m_state.Control;
}

bool EvtData_Key_Pressed_Event::GetShiftButtonPressed() const {
    return m_state.Shift;
}

bool EvtData_Key_Pressed_Event::GetAltButtonPressed() const {
    return m_state.Alt;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Key_Pressed_Event& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    os << "Event key: " << to_underlying(evt.m_state.Key) << std::endl;
    os << "Event char: " << evt.m_state.Char << std::endl;
    os << "Event key state: " << (evt.m_state.State == KeyState::Pressed ? "Released" : "") << std::endl;
    os << "Event control pressed: " << evt.m_state.Control << std::endl;
    os << "Event shift pressed: " << evt.m_state.Shift << std::endl;
    os << "Event alt pressed: " << evt.m_state.Alt << std::endl;
    os.flags(oldFlag);
    return os;
}