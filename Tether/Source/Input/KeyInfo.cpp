#include <Tether/Input/KeyInfo.hpp>

using namespace Tether::Input;

KeyInfo::KeyInfo(uint32_t scancode, uint32_t key, bool pressed)
	:
	scancode(scancode),
	key(key),
	pressed(pressed)
{}

uint32_t KeyInfo::GetScancode()
{
	return scancode;
}

uint32_t KeyInfo::GetKey()
{
	return key;
}

bool KeyInfo::IsPressed()
{
	return pressed;
}
