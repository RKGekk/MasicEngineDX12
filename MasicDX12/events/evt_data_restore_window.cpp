#include "evt_data_restore_window.h"

const std::string EvtData_Restore_Window::sk_EventName = "EvtData_Restore_Window";

EventTypeId EvtData_Restore_Window::VGetEventType() const {
    return sk_EventType;
}

EvtData_Restore_Window::EvtData_Restore_Window() {}

EvtData_Restore_Window::EvtData_Restore_Window(ResizeEventArgs e) : m_state(e.Width, e.Height, e.State) {}

EvtData_Restore_Window::EvtData_Restore_Window(int width, int height, WindowState state) : m_state(width, height, state) {}

void EvtData_Restore_Window::VSerialize(std::ostream& out) const {}

void EvtData_Restore_Window::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Restore_Window::VCopy() const {
    return IEventDataPtr(new EvtData_Restore_Window(m_state));
}

const std::string& EvtData_Restore_Window::GetName() const {
    return sk_EventName;
}

int EvtData_Restore_Window::GetWindowWidth() const {
    return m_state.Width;
}

int EvtData_Restore_Window::GetWindowHeight() const {
    return m_state.Height;
}

WindowState EvtData_Restore_Window::GetWindowState() const {
    return m_state.State;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Restore_Window& evt) {
    std::ios::fmtflags oldFlag = os.flags();
    os << "Event type id: " << evt.sk_EventType << std::endl;
    os << "Event name: " << evt.sk_EventName << std::endl;
    os << "Event time stamp: " << evt.GetTimeStamp().time_since_epoch().count() << "ns" << std::endl;
    switch (evt.m_state.State) {
        case WindowState::Restored: os << "Event window state: restored" << std::endl; break;
        case WindowState::Minimized: os << "Event window state: minimized" << std::endl; break;
        case WindowState::Maximized: os << "Event window state: maximized" << std::endl; break;
        default: break;
    }
    os << "Event window width: " << evt.m_state.Width << std::endl;
    os << "Event window height: " << std::endl << evt.m_state.Height << std::endl;
    os.flags(oldFlag);
    return os;
}