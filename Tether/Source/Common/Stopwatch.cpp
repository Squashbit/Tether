#include <Tether/Common/Stopwatch.hpp>

using namespace Tether;

Stopwatch::Stopwatch()
	:
	startTime(Clock::now())
{}

void Stopwatch::Set()
{
	this->startTime = Clock::now();
}

float Stopwatch::GetElapsedMillis()
{
	std::chrono::duration<float, std::milli> timeSpan = Clock::now() - startTime;
	return timeSpan.count();
}

float Stopwatch::GetElapsedSeconds()
{
	std::chrono::duration<float> timeSpan = Clock::now() - startTime;
	return timeSpan.count();
}
