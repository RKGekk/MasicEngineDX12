#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../tools/string_utility.h"
#include "mouse_button_event_args.h"

class EvtData_Mouse_Button_Released : public BaseEventData {
    MBEventArgs m_state;

public:
    static const EventTypeId sk_EventType = 0x90e19bfd;
    static const std::string sk_EventName;

    EvtData_Mouse_Button_Released();
    EvtData_Mouse_Button_Released(MBEventArgs e);
    EvtData_Mouse_Button_Released(MouseButtonSide button, MKState state, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    // The mouse button that was pressed or released.
    MouseButtonSide GetMouseButton() const;

    // Was the button pressed or released?
    MKState GetButtonState() const;

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

    // The X-position of the cursor relative to the upper-left corner of the client area.
    int GetX() const;

    // The Y-position of the cursor relative to the upper-left corner of the client area.
    int GetY() const;


    friend std::ostream& operator<<(std::ostream& os, const EvtData_Mouse_Button_Released& evt);
};