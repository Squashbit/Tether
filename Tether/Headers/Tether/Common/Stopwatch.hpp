#pragma once

#include <Tether/Common/Defs.hpp>

#include <chrono>

namespace Tether
{
	class TETHER_EXPORT Stopwatch 
	{
	public:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;

		Stopwatch();

		/**
		 * @brief Sets the clock to the current time.
		 *  In other words, this function resets the timer.
		 */
		void Set();

		/**
		 * @returns The time (in milliseconds) since the last time Set was called.
		 */
		float GetElapsedMillis();
		/**
		 * @returns The time (in seconds) since the last time Set was called.
		 */
		float GetElapsedSeconds();
	private:
		TimePoint startTime;
	};
}
