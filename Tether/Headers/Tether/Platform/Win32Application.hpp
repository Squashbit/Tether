#pragma once

#include <Tether/Application.hpp>

namespace Tether::Platform
{
	class TETHER_EXPORT Win32Application : public Application
	{
	private:
		void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes) override;
	};
}