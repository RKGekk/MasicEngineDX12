#pragma once

#include "../window_keycodes.h"

enum class KeyState {
    Released = 0,
    Pressed = 1,
};

struct KeyEventArgs {
    KeyEventArgs();
    KeyEventArgs(WindowKey key, unsigned int c, KeyState state, bool control, bool shift, bool alt);

    WindowKey Key; // The Key Code that was pressed or released.
    unsigned int Char; // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
    KeyState State; // Was the key pressed or released?
    bool Control; // Is the Control modifier pressed
    bool Shift; // Is the Shift modifier pressed
    bool Alt; // Is the Alt modifier pressed
};