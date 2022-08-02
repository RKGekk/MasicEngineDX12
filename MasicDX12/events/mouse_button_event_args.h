#pragma once

enum class MouseButtonSide {
    None = 0,
    Left = 1,
    Right = 2,
    Middle = 3
};

enum class MKState {
    Released = 0,
    Pressed = 1
};

struct MBEventArgs {
    MBEventArgs();
    MBEventArgs(MouseButtonSide btn, MKState state, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y);

    MouseButtonSide Button; // The mouse button that was pressed or released.
    MKState State; // Was the button pressed or released?
    bool LeftButton; // Is the left mouse button down?
    bool MiddleButton; // Is the middle mouse button down?
    bool RightButton; // Is the right mouse button down?
    bool Control; // Is the CTRL key down?
    bool Shift; // Is the Shift key down?

    int X; // The X-position of the cursor relative to the upper-left corner of the client area.
    int Y; // The Y-position of the cursor relative to the upper-left corner of  the client area.
};