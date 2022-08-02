#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../tools/string_utility.h"
#include "mouse_wheel_event_args.h"

class EvtData_Mouse_Wheel : public BaseEventData {
    MouseWheelEventArgs m_stat;

public:
    static const EventTypeId sk_EventType = 0xc368f0fd;
    static const std::string sk_EventName;

    EvtData_Mouse_Wheel();
    EvtData_Mouse_Wheel(MouseWheelEventArgs e);
    EvtData_Mouse_Wheel(float wheel_delta, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.
    float GetWheelDelta() const;

    // Is the left mouse button down?
    bool GetLeftButton() const;

    // Is the middle mouse button down?
    bool GetMiddleButton() const;

    // Is the right mouse button down?
    bool GetRightButton() const;

    // Is the CTRL key down?
    bool GetControl() const;

    // Is the Shift key down?
    bool GetShift() const;

    // The X-position of the cursor relative to the upper-left corner of the client area (in pixels).
    int GetX() const;

    // The Y-position of the cursor relative to the upper-left corner of the client area (in pixels).
    int GetY() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Wheel& evt);
};