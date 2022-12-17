#pragma once

#include <Tether/Common/Defs.hpp>

#include <string>

namespace Tether
{
	namespace Colors
	{
		inline const char* RESET      = "\033[0m";
		inline const char* BLUE       = "\033[38;2;0;120;220m";
		inline const char* GOLD       = "\033[38;2;255;213;0m";
		inline const char* RED        = "\033[38;2;255;0;0m";
		inline const char* GRAY       = "\033[38;2;118;118;118m";
		inline const char* LIGHT_GRAY = "\033[38;2;148;148;148m";
	}

	class TETHER_EXPORT Logger
	{
	public:
		enum class Type
		{
			INFO,
			WARN,
			ERROR
		};

		void SetEnabled(bool enabled);

		void Log(Type type, const std::string& text);
	private:
		std::string GetFormattedTime();
	private:
		bool enabled = true;
	};
}