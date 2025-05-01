#include "Utils.h"
#include "Utils.h"
#include "Utils.h"

std::string CityDraft::Input::Utils::ToString(const Qt::MouseButton& button)
{
	switch (button) {
	case Qt::LeftButton:    return "LMB";
	case Qt::RightButton:   return "RMB";
	case Qt::MiddleButton:  return "MMB";
	case Qt::BackButton:    return "Back";
	case Qt::ForwardButton: return "Forward";
	case Qt::TaskButton:    return "Task";
	case Qt::ExtraButton4:  return "Extra4";
	case Qt::ExtraButton5:  return "Extra5";
	case Qt::ExtraButton6:  return "Extra6";
	case Qt::ExtraButton7:  return "Extra7";
	case Qt::ExtraButton8:  return "Extra8";
	case Qt::ExtraButton9:  return "Extra9";
	case Qt::ExtraButton10: return "Extra10";
	case Qt::ExtraButton11: return "Extra11";
	case Qt::ExtraButton12: return "Extra12";
	case Qt::ExtraButton13: return "Extra13";
	case Qt::ExtraButton14: return "Extra14";
	case Qt::ExtraButton15: return "Extra15";
	case Qt::ExtraButton16: return "Extra16";
	case Qt::ExtraButton17: return "Extra17";
	case Qt::ExtraButton18: return "Extra18";
	case Qt::ExtraButton19: return "Extra19";
	case Qt::ExtraButton20: return "Extra20";
	case Qt::ExtraButton21: return "Extra21";
	case Qt::ExtraButton22: return "Extra22";
	case Qt::ExtraButton23: return "Extra23";
	case Qt::ExtraButton24: return "Extra24";
	case Qt::NoButton:      return "NoButton";
	default:                return "UnknownButton";
	}
}

std::string CityDraft::Input::Utils::ToString(const Qt::KeyboardModifier& modifier)
{
	switch (modifier) {
	case Qt::NoModifier:			return "NoModifier";
	case Qt::ShiftModifier:			return "Shift";
	case Qt::ControlModifier:		return "Ctrl";
	case Qt::AltModifier:			return "Alt";
	case Qt::MetaModifier:			return "Meta";
	case Qt::KeypadModifier:		return "Keypad";
	case Qt::GroupSwitchModifier:	return "GroupSwitch";
	default:						return "UnknownModifier";
	}
}

std::string CityDraft::Input::Utils::ToString(Qt::Key key)
{
	switch (key) {
	case Qt::Key_Enter:         return "Enter";
	case Qt::Key_Return:        return "Return";
	case Qt::Key_Escape:        return "Escape";
	case Qt::Key_Space:         return "Space";
	case Qt::Key_Tab:           return "Tab";
	case Qt::Key_Backtab:       return "Backtab";
	case Qt::Key_Backspace:     return "Backspace";
	case Qt::Key_Delete:        return "Delete";
	case Qt::Key_Left:			return "Left";
	case Qt::Key_Right:			return "Right";
	case Qt::Key_Up:			return "Up";
	case Qt::Key_Down:			return "Down";
	case Qt::Key_Shift:         return "Shift";
	case Qt::Key_Control:       return "Control";
	case Qt::Key_Alt:           return "Alt";
	case Qt::Key_Meta:          return "Meta";
	case Qt::Key_CapsLock:      return "CapsLock";
	case Qt::Key_F1:            return "F1";
	case Qt::Key_F2:            return "F2";
	case Qt::Key_F3:            return "F3";
	case Qt::Key_F4:            return "F4";
	case Qt::Key_F5:            return "F5";
	case Qt::Key_F6:            return "F6";
	case Qt::Key_F7:            return "F7";
	case Qt::Key_F8:            return "F8";
	case Qt::Key_F9:            return "F9";
	case Qt::Key_F10:           return "F10";
	case Qt::Key_F11:           return "F11";
	case Qt::Key_F12:           return "F12";
	case Qt::Key_A:             return "A";
	case Qt::Key_B:             return "B";
	case Qt::Key_C:             return "C";
	case Qt::Key_D:             return "D";
	case Qt::Key_E:             return "E";
	case Qt::Key_F:             return "F";
	case Qt::Key_G:             return "G";
	case Qt::Key_H:             return "H";
	case Qt::Key_I:             return "I";
	case Qt::Key_J:             return "J";
	case Qt::Key_K:             return "K";
	case Qt::Key_L:             return "L";
	case Qt::Key_M:             return "M";
	case Qt::Key_N:             return "N";
	case Qt::Key_O:             return "O";
	case Qt::Key_P:             return "P";
	case Qt::Key_Q:             return "Q";
	case Qt::Key_R:             return "R";
	case Qt::Key_S:             return "S";
	case Qt::Key_T:             return "T";
	case Qt::Key_U:             return "U";
	case Qt::Key_V:             return "V";
	case Qt::Key_W:             return "W";
	case Qt::Key_X:             return "X";
	case Qt::Key_Y:             return "Y";
	case Qt::Key_Z:             return "Z";
	case Qt::Key_0:             return "0";
	case Qt::Key_1:             return "1";
	case Qt::Key_2:             return "2";
	case Qt::Key_3:             return "3";
	case Qt::Key_4:             return "4";
	case Qt::Key_5:             return "5";
	case Qt::Key_6:             return "6";
	case Qt::Key_7:             return "7";
	case Qt::Key_8:             return "8";
	case Qt::Key_9:             return "9";
	case Qt::Key_Slash:         return "Slash";
	case Qt::Key_Asterisk:      return "Asterisk";
	case Qt::Key_Period:        return "Period";
	case Qt::Key_Comma:         return "Comma";
	case Qt::Key_Minus:         return "Minus";
	case Qt::Key_Plus:          return "Plus";
	case Qt::Key_Equal:         return "Equal";
	case Qt::Key_QuoteDbl:      return "\"";
	case Qt::Key_Exclam:        return "!";
	case Qt::Key_At:            return "@";
		// Add more as needed.
	default:                    return "UnknownKey";
	}
}

