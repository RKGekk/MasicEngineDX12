#pragma once

enum class WindowState {
    Restored = 0, // The window has been resized.
    Minimized = 1, // The window has been minimized.
    Maximized = 2, // The window has been maximized.
};

struct ResizeEventArgs {
    // The new width of the window
    int Width;
    // The new height of the window.
    int Height;
    // If the window was minimized or maximized.
    WindowState State;

    ResizeEventArgs();
    ResizeEventArgs(int width, int height, WindowState state);
};