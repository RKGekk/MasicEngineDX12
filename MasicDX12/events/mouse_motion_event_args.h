#pragma once

struct MouseMotionEventArgs {
    MouseMotionEventArgs();
    MouseMotionEventArgs(bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y, int rel_x, int rel_y);

    bool LeftButton; // Is the left mouse button down?
    bool MiddleButton; // Is the middle mouse button down?
    bool RightButton; // Is the right mouse button down?
    bool Control; // Is the CTRL key down?
    bool Shift; // Is the Shift key down?
    int  X; // The X-position of the cursor relative to the upper-left corner of the client area (in pixels).
    int Y; // The Y-position of the cursor relative to the upper-left corner of the client area (in pixels).
    int RelX; // How far the mouse moved since the last event (in pixels).
    int RelY; // How far the mouse moved since the last event (in pixels).
};