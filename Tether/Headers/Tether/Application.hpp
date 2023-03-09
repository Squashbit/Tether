#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Types.hpp>

#include <Tether/Devices/Monitor.hpp>

#include <vector>
#include <optional>
#include <atomic>

namespace Tether
{
	namespace Storage
	{
		struct AppVarStorage;
	}

	class TETHER_EXPORT Application
	{
	public:
		static constexpr const size_t KEYCODES_LENGTH = 512;
		static constexpr const size_t SCANCODES_LENGTH = Keycodes::KEY_LAST + 1;

		Application();
		virtual ~Application() = 0;

		const int16_t*const GetKeycodes() const;
		const int16_t*const GetScancodes() const;

		virtual void Run() = 0;
		bool IsRunning();
		void Stop();

		virtual void PollEvents() = 0;

		virtual size_t GetMonitorCount() = 0;
        virtual std::vector<Devices::Monitor> GetMonitors() = 0;
		
		static Application& Get();
	private:
		std::atomic_bool m_IsRunning = true;
		virtual void CreateKeyLUTs(int16_t* keycodes, int16_t* scancodes) = 0;

		int16_t m_Keycodes[KEYCODES_LENGTH];
		int16_t m_Scancodes[SCANCODES_LENGTH];
		
		inline static Scope<Application> internal = nullptr;
	};
}
