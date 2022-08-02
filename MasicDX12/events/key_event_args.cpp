#include "key_event_args.h"

KeyEventArgs::KeyEventArgs() : Key(WindowKey::None), Char(0u), State(KeyState::Pressed), Control(false), Shift(false), Alt(false) {}

KeyEventArgs::KeyEventArgs(WindowKey key, unsigned int c, KeyState state, bool control, bool shift, bool alt) : Key(key), Char(c), State(state), Control(control), Shift(shift), Alt(alt) {}