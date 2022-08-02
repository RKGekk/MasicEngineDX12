#include "mouse_motion_event_args.h"

MouseMotionEventArgs::MouseMotionEventArgs() : LeftButton(false), MiddleButton(false), RightButton(false), Control(false), Shift(false), X(0), Y(0), RelX(0), RelY(0) {}

MouseMotionEventArgs::MouseMotionEventArgs(bool left_button, bool middle_button, bool right_button, bool control, bool shift, int x, int y, int rel_x, int rel_y) : LeftButton(left_button), MiddleButton(middle_button), RightButton(right_button), Control(control), Shift(shift), X(x), Y(y), RelX(rel_x), RelY(rel_y) {}