#include "Input.h"

BYTE Input::keys[ALL_KEYS] = { 0 };
BYTE Input::oldkeys[ALL_KEYS] = { 0 };
DIMOUSESTATE Input::mouse = { 0 };
DIMOUSESTATE Input::oldmouse = { 0 };
XINPUT_STATE Input::state = {};
XINPUT_STATE Input::oldstate = {};
XINPUT_VIBRATION Input::vibration = {};