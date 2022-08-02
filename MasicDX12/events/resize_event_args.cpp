#include "resize_event_args.h"

ResizeEventArgs::ResizeEventArgs() : Width(1), Height(1), State(WindowState::Restored) {}
ResizeEventArgs::ResizeEventArgs(int width, int height, WindowState state) : Width(width), Height(height), State(state) {}