#pragma once

struct MouseWheelEventArgs {
    MouseWheelEventArgs();
    MouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y);

    float WheelDelta; // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.
    bool LeftButton; // Is the left mouse button down?
    bool MiddleButton; // Is the middle mouse button down?
    bool RightButton; // Is the right mouse button down?
    bool Control; // Is the CTRL key down?
    bool Shift; // Is the Shift key down?
    int X; // The X-position of the cursor relative to the upper-left corner of the client area. 
    int Y; // The Y-position of the cursor relative to the upper-left corner of the client area.
};