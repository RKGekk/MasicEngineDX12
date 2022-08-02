#include "evt_data_resize_window.h"

const std::string EvtData_Resize_Window::sk_EventName = "EvtData_Resize_Window";

EventTypeId EvtData_Resize_Window::VGetEventType() const {
    return sk_EventType;
}

EvtData_Resize_Window::EvtData_Resize_Window() {}

EvtData_Resize_Window::EvtData_Resize_Window(ResizeEventArgs e) : m_state(e.Width, e.Height, e.State) {}

EvtData_Resize_Window::EvtData_Resize_Window(int width, int height, WindowState state) : m_state(width, height, state) {}

void EvtData_Resize_Window::VSerialize(std::ostream& out) const {}

void EvtData_Resize_Window::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Resize_Window::VCopy() const {
    return IEventDataPtr(new EvtData_Resize_Window(m_state));
}

const std::string& EvtData_Resize_Window::GetName() const {
    return sk_EventName;
}

int EvtData_Resize_Window::GetWindowWidth() const {
    return m_state.Width;
}

int EvtData_Resize_Window::GetWindowHeight() const {
    return m_state.Height;
}

WindowState EvtData_Resize_Window::GetWindowState() const {
    return m_state.State;
}

std::ostream& operator<<(std::ostream& os, const EvtData_Resize_Window& evt) {
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