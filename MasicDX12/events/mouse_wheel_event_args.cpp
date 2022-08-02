#include "mouse_wheel_event_args.h"

MouseWheelEventArgs::MouseWheelEventArgs() : WheelDelta(0.0f), LeftButton(false), MiddleButton(false), RightButton(false), Control(false), Shift(false), X(0), Y(0) {}

MouseWheelEventArgs::MouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y) : WheelDelta(wheelDelta), LeftButton(leftButton), MiddleButton(middleButton), RightButton(rightButton), Control(control), Shift(shift), X(x), Y(y) {}