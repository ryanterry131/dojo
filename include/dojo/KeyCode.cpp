#include "KeyCode.h"

namespace Dojo {
	extern KeyCode getKeyCodeForString(utf::string_view name) {
		static const std::map<utf::string_view, KeyCode> mapping = {
			{ "UNASSIGNED", KC_UNASSIGNED },
			{ "ESCAPE", KC_ESCAPE },
			{ "1", KC_1 },
			{ "2", KC_2 },
			{ "3", KC_3 },
			{ "4", KC_4 },
			{ "5", KC_5 },
			{ "6", KC_6 },
			{ "7", KC_7 },
			{ "8", KC_8 },
			{ "9", KC_9 },
			{ "0", KC_0 },
			{ "MINUS", KC_MINUS },
			{ "EQUALS", KC_EQUALS },
			{ "BACK", KC_BACK },
			{ "TAB", KC_TAB },
			{ "Q", KC_Q },
			{ "W", KC_W },
			{ "E", KC_E },
			{ "R", KC_R },
			{ "T", KC_T },
			{ "Y", KC_Y },
			{ "U", KC_U },
			{ "I", KC_I },
			{ "O", KC_O },
			{ "P", KC_P },
			{ "LBRACKET", KC_LBRACKET },
			{ "RBRACKET", KC_RBRACKET },
			{ "RETURN", KC_RETURN },
			{ "LCONTROL", KC_LCONTROL },
			{ "A", KC_A },
			{ "S", KC_S },
			{ "D", KC_D },
			{ "F", KC_F },
			{ "G", KC_G },
			{ "H", KC_H },
			{ "J", KC_J },
			{ "K", KC_K },
			{ "L", KC_L },
			{ "SEMICOLON", KC_SEMICOLON },
			{ "APOSTROPHE", KC_APOSTROPHE },
			{ "GRAVE", KC_GRAVE },
			{ "LSHIFT", KC_LSHIFT },
			{ "BACKSLASH", KC_BACKSLASH },
			{ "Z", KC_Z },
			{ "X", KC_X },
			{ "C", KC_C },
			{ "V", KC_V },
			{ "B", KC_B },
			{ "N", KC_N },
			{ "M", KC_M },
			{ "COMMA", KC_COMMA },
			{ "PERIOD", KC_PERIOD },
			{ "SLASH", KC_SLASH },
			{ "RSHIFT", KC_RSHIFT },
			{ "MULTIPLY", KC_MULTIPLY },
			{ "LEFT_ALT", KC_LEFT_ALT },
			{ "SPACE", KC_SPACE },
			{ "CAPITAL", KC_CAPITAL },
			{ "F1", KC_F1 },
			{ "F2", KC_F2 },
			{ "F3", KC_F3 },
			{ "F4", KC_F4 },
			{ "F5", KC_F5 },
			{ "F6", KC_F6 },
			{ "F7", KC_F7 },
			{ "F8", KC_F8 },
			{ "F9", KC_F9 },
			{ "F10", KC_F10 },
			{ "NUMLOCK", KC_NUMLOCK },
			{ "SCROLL", KC_SCROLL },
			{ "NUMPAD7", KC_NUMPAD7 },
			{ "NUMPAD8", KC_NUMPAD8 },
			{ "NUMPAD9", KC_NUMPAD9 },
			{ "SUBTRACT", KC_SUBTRACT },
			{ "NUMPAD4", KC_NUMPAD4 },
			{ "NUMPAD5", KC_NUMPAD5 },
			{ "NUMPAD6", KC_NUMPAD6 },
			{ "ADD", KC_ADD },
			{ "NUMPAD1", KC_NUMPAD1 },
			{ "NUMPAD2", KC_NUMPAD2 },
			{ "NUMPAD3", KC_NUMPAD3 },
			{ "NUMPAD0", KC_NUMPAD0 },
			{ "DECIMAL", KC_DECIMAL },
			{ "OEM_102", KC_OEM_102 },
			{ "F11", KC_F11 },
			{ "F12", KC_F12 },
			{ "F13", KC_F13 },
			{ "F14", KC_F14 },
			{ "F15", KC_F15 },
			{ "KANA", KC_KANA },
			{ "ABNT_C1", KC_ABNT_C1 },
			{ "CONVERT", KC_CONVERT },
			{ "NOCONVERT", KC_NOCONVERT },
			{ "YEN", KC_YEN },
			{ "ABNT_C2", KC_ABNT_C2 },
			{ "NUMPADEQUALS", KC_NUMPADEQUALS },
			{ "PREVTRACK", KC_PREVTRACK },
			{ "AT", KC_AT },
			{ "COLON", KC_COLON },
			{ "UNDERLINE", KC_UNDERLINE },
			{ "KANJI", KC_KANJI },
			{ "STOP", KC_STOP },
			{ "AX", KC_AX },
			{ "UNLABELED", KC_UNLABELED },
			{ "NEXTTRACK", KC_NEXTTRACK },
			{ "NUMPADENTER", KC_NUMPADENTER },
			{ "RCONTROL", KC_RCONTROL },
			{ "MUTE", KC_MUTE },
			{ "CALCULATOR", KC_CALCULATOR },
			{ "PLAYPAUSE", KC_PLAYPAUSE },
			{ "MEDIASTOP", KC_MEDIASTOP },
			{ "VOLUMEDOWN", KC_VOLUMEDOWN },
			{ "VOLUMEUP", KC_VOLUMEUP },
			{ "WEBHOME", KC_WEBHOME },
			{ "NUMPADCOMMA", KC_NUMPADCOMMA },
			{ "DIVIDE", KC_DIVIDE },
			{ "SYSRQ", KC_SYSRQ },
			{ "RIGHT_ALT", KC_RIGHT_ALT },
			{ "PAUSE", KC_PAUSE },
			{ "HOME", KC_HOME },
			{ "UP", KC_UP },
			{ "PGUP", KC_PGUP },
			{ "LEFT", KC_LEFT },
			{ "RIGHT", KC_RIGHT },
			{ "END", KC_END },
			{ "DOWN", KC_DOWN },
			{ "PGDOWN", KC_PGDOWN },
			{ "INSERT", KC_INSERT },
			{ "DELETE", KC_DELETE },
			{ "LWIN", KC_LWIN },
			{ "RWIN", KC_RWIN },
			{ "APPS", KC_APPS },
			{ "POWER", KC_POWER },
			{ "SLEEP", KC_SLEEP },
			{ "WAKE", KC_WAKE },
			{ "WEBSEARCH", KC_WEBSEARCH },
			{ "WEBFAVORITES", KC_WEBFAVORITES },
			{ "WEBREFRESH", KC_WEBREFRESH },
			{ "WEBSTOP", KC_WEBSTOP },
			{ "WEBFORWARD", KC_WEBFORWARD },
			{ "WEBBACK", KC_WEBBACK },
			{ "MYCOMPUTER", KC_MYCOMPUTER },
			{ "MAIL", KC_MAIL },
			{ "MEDIASELECT", KC_MEDIASELECT },
			{ "JOYPAD_1", KC_JOYPAD_1 },
			{ "JOYPAD_2,", KC_JOYPAD_2, },
			{ "JOYPAD_3,", KC_JOYPAD_3, },
			{ "JOYPAD_4,", KC_JOYPAD_4, },
			{ "JOYPAD_5,", KC_JOYPAD_5, },
			{ "JOYPAD_6,", KC_JOYPAD_6, },
			{ "JOYPAD_7,", KC_JOYPAD_7, },
			{ "JOYPAD_8,", KC_JOYPAD_8, },
			{ "JOYPAD_9,", KC_JOYPAD_9, },
			{ "JOYPAD_10,", KC_JOYPAD_10, },
			{ "JOYPAD_11,", KC_JOYPAD_11, },
			{ "JOYPAD_12,", KC_JOYPAD_12, },
			{ "JOYPAD_13,", KC_JOYPAD_13, },
			{ "JOYPAD_14,", KC_JOYPAD_14, },
			{ "JOYPAD_15,", KC_JOYPAD_15, },
			{ "JOYPAD_16,", KC_JOYPAD_16, },
			{ "JOYPAD_17,", KC_JOYPAD_17, },
			{ "JOYPAD_18,", KC_JOYPAD_18, },
			{ "JOYPAD_19,", KC_JOYPAD_19, },
			{ "JOYPAD_20,", KC_JOYPAD_20, },
			{ "XBOX_DPAD_UP", KC_XBOX_DPAD_UP },
			{ "XBOX_DPAD_DOWN", KC_XBOX_DPAD_DOWN },
			{ "XBOX_DPAD_LEFT", KC_XBOX_DPAD_LEFT },
			{ "XBOX_DPAD_RIGHT", KC_XBOX_DPAD_RIGHT },
			{ "XBOX_START", KC_XBOX_START },
			{ "XBOX_BACK", KC_XBOX_BACK },
			{ "XBOX_LEFT_STICK_PRESS", KC_XBOX_LEFT_STICK_PRESS },
			{ "XBOX_RIGHT_STICK_PRESS", KC_XBOX_RIGHT_STICK_PRESS },
			{ "XBOX_LB", KC_XBOX_LB },
			{ "XBOX_RB", KC_XBOX_RB },
			{ "XBOX_A", KC_XBOX_A },
			{ "XBOX_B", KC_XBOX_B },
			{ "XBOX_X", KC_XBOX_X },
			{ "XBOX_Y", KC_XBOX_Y },
			{ "XBOX_LT", KC_XBOX_LT },
			{ "XBOX_RT", KC_XBOX_RT },
			{ "MOUSE_LEFT", KC_MOUSE_LEFT },
			{ "MOUSE_RIGHT", KC_MOUSE_RIGHT },
			{ "MOUSE_MIDDLE", KC_MOUSE_MIDDLE },
			{ "KEY_COUNT", KC_KEY_COUNT }
		};

		auto elem = mapping.find(name);
		if(elem != mapping.end()) {
			return elem->second;
		}
		DEBUG_ASSERT(elem != mapping.end(), "Invalid configuration");

		return KC_UNASSIGNED;
	}
}