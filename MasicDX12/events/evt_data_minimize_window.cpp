#include "evt_data_minimize_window.h"

const std::string EvtData_Minimize_Window::sk_EventName = "EvtData_Minimize_Window";

EventTypeId EvtData_Minimize_Window::VGetEventType() const {
    return sk_EventType;
}

EvtData_Minimize_Window::EvtData_Minimize_Window() {}

EvtData_Minimize_Window::EvtData_Minimize_Window(ResizeEventArgs e) : m_state(e.Width, e.Height, e.State) {}

EvtData_Minimize_Window::EvtData_Minimize_Window(int width, int height, WindowState state) : m_state(width, height, state) {}

void EvtData_Minimize_Window::VSerialize(std::ostream& out) const {}

void EvtData_Minimize_Window::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Minimize_Window::VCopy() const {
    return IEventDataPtr(new EvtData_Minimize_Window(m_state));
}

const std::string& EvtData_Minimize_Window::GetName() const {
    return sk_EventName;
}

int EvtData_Minimize_Window::GetWindowWidth() const {
    return m_state.Width;
}

int EvtData_Minimize_Window::GetWindowHeight() const {
    return m_state.Height;
}

WindowState EvtData_Minimize_Window::GetWindowState() const {
    return m_state.State;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Minimize_Window& evt) {
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