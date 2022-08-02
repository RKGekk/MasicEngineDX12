#include "mouse_button_event_args.h"

MBEventArgs::MBEventArgs() : Button(MouseButtonSide::None), State(MKState::Pressed), LeftButton(false), MiddleButton(false), RightButton(false), Control(false), Shift(false), X(0), Y(0) {}

MBEventArgs::MBEventArgs(MouseButtonSide button, MKState state, bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y) : Button(button), State(state), LeftButton(left_button), MiddleButton(middle_button), RightButton(right_button), Control(control), Shift(shift), X(x), Y(y) {}