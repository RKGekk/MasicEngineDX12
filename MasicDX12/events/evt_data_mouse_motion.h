#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../tools/string_utility.h"
#include "mouse_motion_event_args.h"

class EvtData_Mouse_Motion : public BaseEventData {
    MouseMotionEventArgs m_state;

public:
    static const EventTypeId sk_EventType = 0xf612a105;
    static const std::string sk_EventName;

    EvtData_Mouse_Motion();
    EvtData_Mouse_Motion(MouseMotionEventArgs e);
    EvtData_Mouse_Motion(bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y, int rel_x, int rel_y);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

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

    // How far the mouse moved since the last event (in pixels).
    int GetRelX() const;

    // How far the mouse moved since the last event (in pixels).
    int GetRelY() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Motion& evt);
};