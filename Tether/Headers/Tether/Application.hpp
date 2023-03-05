#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>
#include <optional>

namespace Tether
{
	namespace Storage
	{
		struct AppVarStorage;
	}

	class TETHER_EXPORT Application
    {
    public:
        Application() = default;
        virtual ~Application() = 0;
        
        const int16_t*const GetKeycodes() const;
        const int16_t*const GetScancodes() const;
        
        static Application& Get();
    protected:
		int16_t keycodes[512];
		int16_t scancodes[Keycodes::KEY_LAST + 1];
    private:
        inline static Scope<Application> internal = nullptr;
    };
}
