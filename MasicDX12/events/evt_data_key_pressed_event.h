#pragma once

#include <iostream>
#include <iomanip>

#include <DirectXMath.h>

#include "base_event_data.h"
#include "../tools/string_utility.h"
#include "key_event_args.h"

class EvtData_Key_Pressed_Event : public BaseEventData {
    KeyEventArgs m_state;

public:
    static const EventTypeId sk_EventType = 0x57dbe096;
    static const std::string sk_EventName;

    EvtData_Key_Pressed_Event();
    EvtData_Key_Pressed_Event(KeyEventArgs e);
    EvtData_Key_Pressed_Event(WindowKey key, unsigned int c, KeyState state, bool control, bool shift, bool alt);

    virtual EventTypeId VGetEventType() const override;
    virtual void VSerialize(std::ostream& out) const override;
    virtual void VDeserialize(std::istream& in) override;
    virtual IEventDataPtr VCopy() const override;
    virtual const std::string& GetName() const override;

    // The Key Code that was pressed or released.
    WindowKey GetWindowKeyCode() const;

    // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
    unsigned int GetCharCode() const;
    
    // Was the key pressed or released?
    KeyState GetKeyPressState() const;
    
    // Is the Control modifier pressed
    bool GetControlButtonPressed() const;
    
    // Is the Shift modifier pressed
    bool GetShiftButtonPressed() const;
    
    // Is the Alt modifier pressed
    bool GetAltButtonPressed() const;

    friend std::ostream& operator<<(std::ostream& os, const EvtData_Key_Pressed_Event& evt);
};