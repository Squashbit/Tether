#include <Tether/Platform/Win32Application.hpp>
#include <cstring>

namespace Tether::Platform
{
	void Win32Application::CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes)
	{
		keycodes[0x00B] = Keycodes::KEY_0;
		keycodes[0x002] = Keycodes::KEY_1;
		keycodes[0x003] = Keycodes::KEY_2;
		keycodes[0x004] = Keycodes::KEY_3;
		keycodes[0x005] = Keycodes::KEY_4;
		keycodes[0x006] = Keycodes::KEY_5;
		keycodes[0x007] = Keycodes::KEY_6;
		keycodes[0x008] = Keycodes::KEY_7;
		keycodes[0x009] = Keycodes::KEY_8;
		keycodes[0x00A] = Keycodes::KEY_9;
		keycodes[0x01E] = Keycodes::KEY_A;
		keycodes[0x030] = Keycodes::KEY_B;
		keycodes[0x02E] = Keycodes::KEY_C;
		keycodes[0x020] = Keycodes::KEY_D;
		keycodes[0x012] = Keycodes::KEY_E;
		keycodes[0x021] = Keycodes::KEY_F;
		keycodes[0x022] = Keycodes::KEY_G;
		keycodes[0x023] = Keycodes::KEY_H;
		keycodes[0x017] = Keycodes::KEY_I;
		keycodes[0x024] = Keycodes::KEY_J;
		keycodes[0x025] = Keycodes::KEY_K;
		keycodes[0x026] = Keycodes::KEY_L;
		keycodes[0x032] = Keycodes::KEY_M;
		keycodes[0x031] = Keycodes::KEY_N;
		keycodes[0x018] = Keycodes::KEY_O;
		keycodes[0x019] = Keycodes::KEY_P;
		keycodes[0x010] = Keycodes::KEY_Q;
		keycodes[0x013] = Keycodes::KEY_R;
		keycodes[0x01F] = Keycodes::KEY_S;
		keycodes[0x014] = Keycodes::KEY_T;
		keycodes[0x016] = Keycodes::KEY_U;
		keycodes[0x02F] = Keycodes::KEY_V;
		keycodes[0x011] = Keycodes::KEY_W;
		keycodes[0x02D] = Keycodes::KEY_X;
		keycodes[0x015] = Keycodes::KEY_Y;
		keycodes[0x02C] = Keycodes::KEY_Z;

		keycodes[0x028] = Keycodes::KEY_APOSTROPHE;
		keycodes[0x02B] = Keycodes::KEY_BACKSLASH;
		keycodes[0x033] = Keycodes::KEY_COMMA;
		keycodes[0x00D] = Keycodes::KEY_EQUAL;
		keycodes[0x029] = Keycodes::KEY_GRAVE_ACCENT;
		keycodes[0x01A] = Keycodes::KEY_LEFT_BRACKET;
		keycodes[0x00C] = Keycodes::KEY_MINUS;
		keycodes[0x034] = Keycodes::KEY_PERIOD;
		keycodes[0x01B] = Keycodes::KEY_RIGHT_BRACKET;
		keycodes[0x027] = Keycodes::KEY_SEMICOLON;
		keycodes[0x035] = Keycodes::KEY_SLASH;
		keycodes[0x056] = Keycodes::KEY_WORLD_2;

		keycodes[0x00E] = Keycodes::KEY_BACKSPACE;
		keycodes[0x153] = Keycodes::KEY_DELETE;
		keycodes[0x14F] = Keycodes::KEY_END;
		keycodes[0x01C] = Keycodes::KEY_ENTER;
		keycodes[0x001] = Keycodes::KEY_ESCAPE;
		keycodes[0x147] = Keycodes::KEY_HOME;
		keycodes[0x152] = Keycodes::KEY_INSERT;
		keycodes[0x15D] = Keycodes::KEY_MENU;
		keycodes[0x151] = Keycodes::KEY_PAGE_DOWN;
		keycodes[0x149] = Keycodes::KEY_PAGE_UP;
		keycodes[0x045] = Keycodes::KEY_PAUSE;
		keycodes[0x039] = Keycodes::KEY_SPACE;
		keycodes[0x00F] = Keycodes::KEY_TAB;
		keycodes[0x03A] = Keycodes::KEY_CAPS_LOCK;
		keycodes[0x145] = Keycodes::KEY_NUM_LOCK;
		keycodes[0x046] = Keycodes::KEY_SCROLL_LOCK;
		keycodes[0x03B] = Keycodes::KEY_F1;
		keycodes[0x03C] = Keycodes::KEY_F2;
		keycodes[0x03D] = Keycodes::KEY_F3;
		keycodes[0x03E] = Keycodes::KEY_F4;
		keycodes[0x03F] = Keycodes::KEY_F5;
		keycodes[0x040] = Keycodes::KEY_F6;
		keycodes[0x041] = Keycodes::KEY_F7;
		keycodes[0x042] = Keycodes::KEY_F8;
		keycodes[0x043] = Keycodes::KEY_F9;
		keycodes[0x044] = Keycodes::KEY_F10;
		keycodes[0x057] = Keycodes::KEY_F11;
		keycodes[0x058] = Keycodes::KEY_F12;
		keycodes[0x064] = Keycodes::KEY_F13;
		keycodes[0x065] = Keycodes::KEY_F14;
		keycodes[0x066] = Keycodes::KEY_F15;
		keycodes[0x067] = Keycodes::KEY_F16;
		keycodes[0x068] = Keycodes::KEY_F17;
		keycodes[0x069] = Keycodes::KEY_F18;
		keycodes[0x06A] = Keycodes::KEY_F19;
		keycodes[0x06B] = Keycodes::KEY_F20;
		keycodes[0x06C] = Keycodes::KEY_F21;
		keycodes[0x06D] = Keycodes::KEY_F22;
		keycodes[0x06E] = Keycodes::KEY_F23;
		keycodes[0x076] = Keycodes::KEY_F24;
		keycodes[0x038] = Keycodes::KEY_LEFT_ALT;
		keycodes[0x01D] = Keycodes::KEY_LEFT_CONTROL;
		keycodes[0x02A] = Keycodes::KEY_LEFT_SHIFT;
		keycodes[0x15B] = Keycodes::KEY_LEFT_SUPER;
		keycodes[0x137] = Keycodes::KEY_PRINT_SCREEN;
		keycodes[0x138] = Keycodes::KEY_RIGHT_ALT;
		keycodes[0x11D] = Keycodes::KEY_RIGHT_CONTROL;
		keycodes[0x036] = Keycodes::KEY_RIGHT_SHIFT;
		keycodes[0x15C] = Keycodes::KEY_RIGHT_SUPER;
		keycodes[0x150] = Keycodes::KEY_DOWN;
		keycodes[0x14B] = Keycodes::KEY_LEFT;
		keycodes[0x14D] = Keycodes::KEY_RIGHT;
		keycodes[0x148] = Keycodes::KEY_UP;

		keycodes[0x052] = Keycodes::KEY_KP_0;
		keycodes[0x04F] = Keycodes::KEY_KP_1;
		keycodes[0x050] = Keycodes::KEY_KP_2;
		keycodes[0x051] = Keycodes::KEY_KP_3;
		keycodes[0x04B] = Keycodes::KEY_KP_4;
		keycodes[0x04C] = Keycodes::KEY_KP_5;
		keycodes[0x04D] = Keycodes::KEY_KP_6;
		keycodes[0x047] = Keycodes::KEY_KP_7;
		keycodes[0x048] = Keycodes::KEY_KP_8;
		keycodes[0x049] = Keycodes::KEY_KP_9;
		keycodes[0x04E] = Keycodes::KEY_KP_ADD;
		keycodes[0x053] = Keycodes::KEY_KP_DECIMAL;
		keycodes[0x135] = Keycodes::KEY_KP_DIVIDE;
		keycodes[0x11C] = Keycodes::KEY_KP_ENTER;
		keycodes[0x059] = Keycodes::KEY_KP_EQUAL;
		keycodes[0x037] = Keycodes::KEY_KP_MULTIPLY;
		keycodes[0x04A] = Keycodes::KEY_KP_SUBTRACT;

		for (uint16_t scancode = 0; scancode < 512; scancode++)
		{
			if (keycodes[scancode] > 0)
				scancodes[keycodes[scancode]] = scancode;
		}
	}
}
