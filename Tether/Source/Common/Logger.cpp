#include <Tether/Common/Logger.hpp>

#include <iostream>
#include <chrono>
#include <ctime>

using namespace Tether;

void Logger::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}

void Logger::Log(Type type, const std::string& text)
{
	if (!enabled)
		return;

	std::string prefix;
	switch (type)
	{
		case Logger::Type::INFO: prefix = Colors::BLUE; prefix += "[INFO] => "; break;
		case Logger::Type::WARN: prefix = Colors::GOLD; prefix += "[WARN] => "; break;
		case Logger::Type::ERROR: prefix = Colors::RED; prefix += "[ERROR] => "; break;
	}

	std::string message;
	message += GetFormattedTime();
	message += prefix;
	message += text;
	message += Colors::RESET;

	std::cout << message << std::endl;
}

std::string Logger::GetFormattedTime()
{
	std::string time;
	time += Colors::GRAY;
	time += "[";
	time += Colors::LIGHT_GRAY;

	auto now = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

	std::tm organizedTime{};
	gmtime_s(&organizedTime, &currentTime);

	time += std::to_string(organizedTime.tm_hour > 12 ?
		organizedTime.tm_hour - 12 : organizedTime.tm_hour);
	time += ":";
	time += std::to_string(organizedTime.tm_min);
	time += ":";
	time += std::to_string(organizedTime.tm_sec);

	time += Colors::GRAY;
	time += "] ";
	time += Colors::RESET;
	time += ": ";

	return time;
}
