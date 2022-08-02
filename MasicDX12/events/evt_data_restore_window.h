#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "resize_event_args.h"
#include "../tools/string_utility.h"

class EvtData_Restore_Window : public BaseEventData {
    ResizeEventArgs m_state;

public:
    static const EventTypeId sk_EventType = 0x0a6229aa;
    static const std::string sk_EventName;

    EvtData_Restore_Window();
    EvtData_Restore_Window(ResizeEventArgs e);
    EvtData_Restore_Window(int width, int height, WindowState state);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    // The new width of the window
    int GetWindowWidth() const;

    // The new height of the window
    int GetWindowHeight() const;

    // If the window was minimized or maximized.
    WindowState GetWindowState() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Restore_Window& evt);
};